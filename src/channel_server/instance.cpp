/*
Copyright (C) 2008-2016 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "instance.hpp"
#include "common/timer/container.hpp"
#include "common/timer/timer.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instances.hpp"
#include "channel_server/lua/lua_instance.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/party.hpp"
#include "channel_server/player.hpp"
#include "channel_server/reactor.hpp"
#include <functional>
#include <sstream>
#include <utility>

namespace vana {
namespace channel_server {

instance::instance(const string &name, game_map_id map, game_player_id player_id, const duration &time, const duration &persistent, bool show_timer) :
	m_name{name},
	m_persistent{persistent},
	m_show_timer{show_timer},
	m_start{vana::util::time::get_now()}
{
	m_variables = make_owned_ptr<variables>();
	m_lua_instance = make_owned_ptr<lua::lua_instance>(name, player_id);

	if (player_id != 0) {
		channel_server::get_instance().log(vana::log::type::instance_begin, [&](out_stream &log) { log << name << " started by player ID " << player_id; });
	}
	set_instance_timer(time, true);
}

instance::~instance() {
	// Maps
	for (const auto &map : m_maps) {
		map->end_instance(m_reset_on_destroy);
	}
	m_maps.clear();

	// Parties
	for (const auto &party : m_parties) {
		party->set_instance(nullptr);
	}
	m_parties.clear();

	// Players
	for (const auto &kvp : m_players) {
		kvp.second->set_instance(nullptr);
	}
	m_players.clear();
	channel_server::get_instance().get_instances().remove_instance(this);
}

auto instance::add_player(ref_ptr<player> player) -> void {
	if (player != nullptr) {
		m_players[player->get_id()] = player;
		player->set_instance(this);
	}
}

auto instance::remove_player(ref_ptr<player> player) -> void {
	remove_player(player->get_id());
	player->set_instance(nullptr);
}

auto instance::remove_player(game_player_id id) -> void {
	auto kvp = m_players.find(id);
	if (kvp != std::end(m_players)) {
		m_players.erase(kvp);
	}
}

auto instance::remove_all_players() -> void {
	auto copy = m_players;
	for (const auto &kvp : copy) {
		remove_player(kvp.second);
	}
}

auto instance::move_all_players(game_map_id map_id, bool respect_instances, const data::type::portal_info * const portal) -> void {
	if (!maps::get_map(map_id)) {
		return;
	}
	// Copy in the event that we don't respect instances
	auto copy = m_players;
	for (const auto &kvp : copy) {
		kvp.second->set_map(map_id, portal, respect_instances);
	}
}

auto instance::get_all_player_ids() -> vector<game_player_id> {
	vector<game_player_id> player_ids;
	for (const auto &kvp : m_players) {
		player_ids.push_back(kvp.first);
	}
	return player_ids;
}

auto instance::instance_has_players() const -> bool {
	for (const auto &map : m_maps) {
		if (map->get_num_players() != 0) {
			return true;
		}
	}
	return false;
}

auto instance::add_map(map *map) -> void {
	m_maps.push_back(map);
	map->set_instance(this);
}

auto instance::add_map(game_map_id map_id) -> void {
	map *map = maps::get_map(map_id);
	add_map(map);
}

auto instance::is_instance_map(game_map_id map_id) const -> bool {
	for (const auto &map : m_maps) {
		if (map->get_id() == map_id) {
			return true;
		}
	}
	return false;
}

auto instance::add_party(party *party) -> void {
	m_parties.push_back(party);
	party->set_instance(this);
}

auto instance::add_future_timer(const string &timer_name, seconds time, seconds persistence) -> bool {
	if (timer_name == "instance") {
		set_instance_timer(time, false);
		return true;
	}

	if (m_timer_actions.find(timer_name) == std::end(m_timer_actions)) {
		timer_action timer;
		timer.counter_id = get_counter_id();
		timer.is_persistent = persistence.count() > 0;
		m_timer_actions.emplace(timer_name, timer);

		vana::timer::id id{vana::timer::type::instance_timer, timer.counter_id};
		vana::timer::timer::create([this, timer_name](const time_point &now) { this->timer_complete(timer_name, true); },
			id, get_timers(), time, persistence);

		return true;
	}
	return false;
}

auto instance::add_second_of_hour_timer(const string &timer_name, int16_t second_of_hour, seconds persistence) -> bool {
	if (m_timer_actions.find(timer_name) == std::end(m_timer_actions)) {
		timer_action timer;
		timer.counter_id = get_counter_id();
		timer.is_persistent = persistence.count() > 0;
		m_timer_actions.emplace(timer_name, timer);

		vana::timer::id id{vana::timer::type::instance_timer, timer.counter_id};
		vana::timer::timer::create([this, timer_name](const time_point &now) { this->timer_complete(timer_name, true); },
			id, get_timers(), vana::util::time::get_distance_to_next_occurring_second_of_hour(second_of_hour), persistence);

		return true;
	}
	return false;
}

auto instance::get_timer_seconds_remaining(const string &timer_name) -> seconds {
	seconds time_left{0};
	auto kvp = m_timer_actions.find(timer_name);
	if (kvp != std::end(m_timer_actions)) {
		auto &timer = kvp->second;
		vana::timer::id id{vana::timer::type::instance_timer, timer.counter_id};
		time_left = get_timers()->get_remaining_time<seconds>(id);
	}
	return time_left;
}

auto instance::remove_timer(const string &timer_name) -> void {
	if (timer_name == "instance") {
		instance_end(true, false);
		return;
	}
	remove_timer(timer_name, true);
}

auto instance::remove_timer(const string &timer_name, bool perform_event) -> void {
	auto kvp = m_timer_actions.find(timer_name);
	if (kvp != std::end(m_timer_actions)) {
		const timer_action &timer = kvp->second;
		if (get_timer_seconds_remaining(timer_name).count() > 0) {
			vana::timer::id id{vana::timer::type::instance_timer, timer.counter_id};
			get_timers()->remove_timer(id);
			if (perform_event) {
				timer_end(timer_name, false);
			}
		}
		m_timer_actions.erase(kvp);
	}
}

auto instance::remove_all_timers() -> void {
	auto copy = m_timer_actions;
	for (const auto &kvp : copy) {
		remove_timer(kvp.first);
	}
}

auto instance::get_instance_seconds_remaining() -> seconds {
	return get_timer_seconds_remaining("instance");
}

auto instance::set_instance_timer(const duration &time, bool first_run) -> void {
	if (get_instance_seconds_remaining().count() > 0) {
		remove_timer("instance", false);
	}

	if (time.count() != 0) {
		timer_action timer;
		timer.counter_id = get_counter_id();
		timer.is_persistent = m_persistent.count() > 0;
		m_timer_actions.emplace("instance", timer);

		vana::timer::id id{vana::timer::type::instance_timer, timer.counter_id};
		vana::timer::timer::create(
			[this](const time_point &now) {
				this->instance_end(false, true);
			},
			id,
			get_timers(),
			time,
			m_persistent);

		if (!first_run && show_timer()) {
			show_timer(true, true);
		}
	}
}

auto instance::begin_instance() -> result {
	return call_instance_function("beginInstance");
}

auto instance::player_death(game_player_id player_id) -> result {
	return call_instance_function("playerDeath", player_id);
}

auto instance::party_disband(game_party_id party_id) -> result {
	return call_instance_function("partyDisband", party_id);
}

auto instance::timer_end(const string &name, bool from_timer) -> result {
	return call_instance_function("timerEnd", name, from_timer);
}

auto instance::player_disconnect(game_player_id player_id, bool is_party_leader) -> result {
	return call_instance_function("playerDisconnect", player_id, is_party_leader);
}

auto instance::remove_party_member(game_party_id party_id, game_player_id player_id) -> result {
	return call_instance_function("partyRemoveMember", party_id, player_id);
}

auto instance::mob_death(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id) -> result {
	return call_instance_function("mobDeath", mob_id, map_mob_id, map_id);
}

auto instance::mob_spawn(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id) -> result {
	return call_instance_function("mobSpawn", mob_id, map_mob_id, map_id);
}

auto instance::player_change_map(game_player_id player_id, game_map_id new_map_id, game_map_id old_map_id, bool is_party_leader) -> result {
	return call_instance_function("changeMap", player_id, new_map_id, old_map_id, is_party_leader);
}

auto instance::friendly_mob_hit(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id, int32_t mob_hp, int32_t mob_max_hp) -> result {
	return call_instance_function("friendlyHit", mob_id, map_mob_id, map_id, mob_hp, mob_max_hp);
}

auto instance::timer_complete(const string &name, bool from_timer) -> void {
	timer_end(name, from_timer);
	if (!from_timer || (from_timer && !is_timer_persistent(name))) {
		remove_timer(name);
	}
}

auto instance::instance_end(bool called_by_lua, bool from_timer) -> void {
	if (!called_by_lua) {
		timer_end("instance", from_timer);
	}

	if (!from_timer || (from_timer && m_persistent.count() > 0)) {
		remove_timer("instance", false);
	}

	show_timer(false);
	if (m_persistent.count() == 0) {
		mark_for_delete();
	}
}

auto instance::is_timer_persistent(const string &name) -> bool {
	auto iter = m_timer_actions.find(name);
	return iter != std::end(m_timer_actions) ?
		iter->second.is_persistent :
		false;
}

auto instance::get_counter_id() -> uint32_t {
	return ++m_timer_counter;
}

auto instance::mark_for_delete() -> void {
	m_marked_for_deletion = true;
	clear_timers();
	for (const auto &map : m_maps) {
		map->set_instance(nullptr);
	}
	for (const auto &player : m_players) {
		player.second->set_instance(nullptr);
	}
	m_players.clear();
	for (const auto &party : m_parties) {
		party->set_instance(nullptr);
	}
	m_parties.clear();

	// TODO FIXME lua
	// TODO FIXME instance
	// All sorts of instance trickery needs to be cleaned up here...perhaps marking for deletion needs a full review
	// e.g. a crash can be caused by marking for delete and then going to a map where an instance was (e.g. Zakum signup)
}

auto instance::respawn_mobs(game_map_id map_id) -> void {
	if (map_id == constant::map::no_map) {
		for (const auto &map : m_maps) {
			map->respawn(spawn_types::mob);
		}
	}
	else {
		maps::get_map(map_id)->respawn(spawn_types::mob);
	}
}

auto instance::respawn_reactors(game_map_id map_id) -> void {
	if (map_id == constant::map::no_map) {
		for (const auto &map : m_maps) {
			map->respawn(spawn_types::reactor);
		}
	}
	else {
		maps::get_map(map_id)->respawn(spawn_types::reactor);
	}
}

auto instance::show_timer(bool show, bool do_it) -> void {
	if (!show && (do_it || m_show_timer)) {
		for (const auto &map : m_maps) {
			map->send(packets::map::show_timer(seconds{0}));
		}
	}
	else if (show && (do_it || !m_show_timer)) {
		for (const auto &map : m_maps) {
			map->send(packets::map::show_timer(get_instance_seconds_remaining()));
		}
	}
}

auto instance::set_persistence(const duration &persistence) -> void {
	m_persistent = persistence;
}

auto instance::get_persistence() const -> duration {
	return m_persistent;
}

auto instance::show_timer() const -> bool {
	return m_show_timer;
}

}
}