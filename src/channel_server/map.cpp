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
#include "map.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/npc.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/misc_utilities.hpp"
#include "common/packet_wrapper.hpp"
#include "common/randomizer.hpp"
#include "common/session.hpp"
#include "common/split_packet_builder.hpp"
#include "common/time_utilities.hpp"
#include "common/timer/timer.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/drop.hpp"
#include "channel_server/effect_packet.hpp"
#include "channel_server/gm_packet.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/maple_tv_packet.hpp"
#include "channel_server/maple_tvs.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mist.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/mob_handler.hpp"
#include "channel_server/mobs_packet.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/npc_packet.hpp"
#include "channel_server/party.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/reactor_packet.hpp"
#include "channel_server/reactor.hpp"
#include "channel_server/summon_handler.hpp"
#include <ctime>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace vana {
namespace channel_server {

// TODO FIXME msvc
// Remove this crap once MSVC supports static initializers
int32_t map::s_map_unload_time = 0;

map::map(ref_ptr<const data::type::map_info> info, game_map_id id) :
	m_info{info},
	m_id{id},
	m_object_ids{1000},
	m_music{info->default_music}
{
	point right_bottom = info->dimensions.right_bottom();
	double map_height = std::max<double>(right_bottom.y - 450, 600);
	double map_width = std::max<double>(right_bottom.x, 800);
	m_min_spawn_count = ext::constrain_range(static_cast<int32_t>((map_height * map_width * info->spawn_rate) / 128000.), 1, 40);
	m_max_spawn_count = m_min_spawn_count * 2;
	m_run_unloader = info->ship_kind == -1;
	m_infer_size_from_footholds = info->dimensions.area() == 0;
	if (!m_infer_size_from_footholds) {
		m_real_dimensions = info->dimensions;
	}

	for (const auto &foothold : info->link_info->footholds) add_foothold(foothold);
	for (const auto &mob : info->link_info->mobs) add_mob_spawn(mob);
	for (const auto &npc : info->link_info->npcs) add_npc(npc);
	for (const auto &portal : info->link_info->portals) add_portal(portal);
	for (const auto &reactor : info->link_info->reactors) add_reactor_spawn(reactor);
	for (const auto &seat : info->link_info->seats) add_seat(seat);
	if (info->link_info->time_mob.is_initialized()) add_time_mob(info->link_info->time_mob.get());

	// Dynamic loading, start the map timer once the object is created
	vana::timer::timer::create(
		[this](const time_point &now) { this->map_tick(now); },
		vana::timer::id{vana::timer::type::map_timer, m_info->id},
		get_timers(), seconds{0}, seconds{1});
}

// Map info
auto map::set_music(const string &music_name) -> void {
	m_music = music_name == "default" ?
		m_info->default_music :
		music_name;

	send(packets::play_music(m_music));
}

auto map::set_mob_spawning(game_mob_id spawn) -> void {
	m_spawn_mobs = spawn;
}

auto map::make_npc_id(game_map_object received_id) -> size_t {
	return received_id - npc_start;
}

auto map::make_npc_id() -> game_map_object {
	return m_npc_spawns.size() - 1 + npc_start;
}

auto map::make_reactor_id(game_map_object received_id) -> size_t {
	return received_id - reactor_start;
}

auto map::make_reactor_id() -> game_map_object {
	return m_reactor_spawns.size() - 1 + reactor_start;
}

// Data initialization
auto map::add_foothold(const data::type::foothold_info &foothold) -> void {
	m_footholds.push_back(foothold);
	if (m_infer_size_from_footholds) {
		m_real_dimensions = m_real_dimensions.combine(foothold.line.make_rect());
	}
}

auto map::add_seat(const data::type::seat_info &seat) -> void {
	map_seat record;
	record.info = seat;
	m_seats[seat.id] = record;
}

auto map::add_reactor_spawn(const data::type::reactor_spawn_info &spawn) -> void {
	m_reactor_spawns.push_back(spawn);
	m_reactor_spawns[m_reactor_spawns.size() - 1].spawned = true;
	reactor *value = new reactor{get_id(), spawn.id, spawn.pos, spawn.faces_left};
	send(packets::spawn_reactor(value));
	add_reactor(value);
}

auto map::add_mob_spawn(const data::type::mob_spawn_info &spawn) -> void {
	m_mob_spawns.push_back(spawn);
	m_mob_spawns[m_mob_spawns.size() - 1].spawned = true;
	auto info = channel_server::get_instance().get_mob_data_provider().get_mob_info(spawn.id);
	if (info->boss) {
		m_run_unloader = false;
	}
	m_max_mob_spawn_time = std::max(m_max_mob_spawn_time, spawn.time);
	spawn_mob(m_mob_spawns.size() - 1, spawn);
}

auto map::add_portal(const data::type::portal_info &portal) -> void {
	if (portal.name == "sp") {
		m_spawn_points[portal.id] = portal;
	}
	else if (portal.name == "tp") {
		m_door_points.push_back(portal);
	}
	else {
		m_portals[portal.name] = portal;
	}
}

auto map::add_time_mob(data::type::time_mob_info info) -> void {
	vana::timer::timer::create([this](const time_point &now) { this->check_time_mob_spawn(false); },
		vana::timer::id{vana::timer::type::map_timer, get_id(), 1},
		get_timers(), utilities::time::get_distance_to_next_occurring_second_of_hour(0), hours{1});

	vana::timer::timer::create([this](const time_point &now) { this->check_time_mob_spawn(true); },
		vana::timer::id{vana::timer::type::map_timer, get_id(), 2},
		get_timers(), seconds{3}); // First check
}

// Players
auto map::add_player(ref_ptr<player> player) -> void {
	m_players.push_back(player);
	if (m_info->force_map_equip) {
		player->send(packets::map::force_map_equip());
	}
	if (!player->is_using_gm_hide()) {
		send(packets::map::player_packet(player), player);
	}
	else {
		player->send(packets::gm::begin_hide());
	}
	if (m_timer.count() > 0) {
		player->send(packets::map::show_timer(m_timer - duration_cast<seconds>(utilities::time::get_now() - m_timer_start)));
	}
	else if (instance *inst = get_instance()) {
		if (inst->show_timer()) {
			seconds time = inst->get_instance_seconds_remaining();
			if (time.count() > 0) {
				player->send(packets::map::show_timer(time));
			}
		}
	}
	if (m_info->ship_kind != -1) {
		player->send(packets::map::boat_dock_update(m_ship, m_info->ship_kind));
	}

	if (player->is_stalking()) {
		player->send(packets::player::stalk_result(player));
	}

	packet_builder stalking_addition_packet = packets::player::stalk_add_or_update_player(player);
	run_function_players([&](ref_ptr<vana::channel_server::player> p) {
		if (p->is_stalking() && p != player) {
			p->send(stalking_addition_packet);
		}
	});

	check_player_equip(player);
}

auto map::check_player_equip(ref_ptr<player> player) -> void {
	if (!player->has_gm_benefits()) {
		game_damage dps = m_info->damage_per_second;
		if (dps > 0) {
			game_item_id protect_item = m_info->protect_item;
			game_player_id player_id = player->get_id();
			if (protect_item > 0) {
				if (!player->get_inventory()->is_equipped_item(protect_item)) {
					m_players_without_protect_item[player_id] = player;
				}
				else {
					auto kvp = m_players_without_protect_item.find(player_id);
					if (kvp != std::end(m_players_without_protect_item)) {
						m_players_without_protect_item.erase(kvp);
					}
				}
			}
			else {
				m_players_without_protect_item[player_id] = player;
			}
		}
	}
}

auto map::boat_dock(bool is_docked) -> void {
	if (is_docked != m_ship) {
		m_ship = is_docked;
		send(packets::map::boat_dock_update(is_docked, m_info->ship_kind));
	}
}

auto map::set_map_unload_time(seconds new_time) -> void {
	s_map_unload_time = static_cast<int32_t>(new_time.count());
}

auto map::get_num_players() const -> size_t {
	return m_players.size();
}

auto map::get_player(size_t index) const -> ref_ptr<player> {
	return m_players[index];
}

auto map::get_player_names() -> string {
	out_stream names;
	for (size_t i = 0; i < m_players.size(); i++) {
		if (auto player = m_players[i]) {
			names << player->get_name() << " ";
		}
	}
	return names.str();
}

auto map::remove_player(ref_ptr<player> player) -> void {
	game_player_id player_id = player->get_id();
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players[i] == player) {
			m_players.erase(std::begin(m_players) + i);
			break;
		}
	}

	player->get_active_buffs()->reset_homing_beacon_mob();

	player->get_summons()->for_each([player](summon *summon) {
		summon_handler::remove_summon(player, summon->get_id(), true, summon_messages::none);
	});

	send(packets::map::remove_player(player_id), player);
	update_mob_control(player);

	auto kvp = m_players_without_protect_item.find(player_id);
	if (kvp != std::end(m_players_without_protect_item)) {
		m_players_without_protect_item.erase(kvp);
	}

	packet_builder stalking_removal_packet = packets::player::stalk_remove_player(vector<game_player_id>{ player_id });
	run_function_players([stalking_removal_packet, player](ref_ptr<vana::channel_server::player> p) {
		if (p->is_stalking() && p != player) {
			p->send(stalking_removal_packet);
		}
	});

	player->send(packets::player::stalk_remove_player(get_all_player_ids()));
}

auto map::run_function_players(const rect &dimensions, int16_t prop, function<void(ref_ptr<player>)> success_func) -> void {
	run_function_players(dimensions, prop, 0, success_func);
}

auto map::run_function_players(const rect &dimensions, int16_t prop, int16_t count, function<void(ref_ptr<player>)> success_func) -> void {
	int16_t done = 0;
	// Prevent iterator invalidation
	auto copy = m_players;
	for (const auto &player : copy) {
		if (dimensions.contains(player->get_pos()) && randomizer::percentage<int16_t>() < prop) {
			success_func(player);
			done++;
		}
		if (count > 0 && done == count) {
			break;
		}
	}
}

auto map::run_function_players(function<void(ref_ptr<player>)> success_func) -> void {
	// Prevent iterator invalidation
	auto copy = m_players;
	for (const auto &player : copy) {
		success_func(player);
	}
}

auto map::buff_players(game_item_id buff_id) -> void {
	for (const auto &player : m_players) {
		if (player->get_stats()->get_hp() > 0) {
			inventory::use_item(player, buff_id);
			player->send_map(packets::send_mob_item_buff_effect(player->get_id(), buff_id));
		}
	}
}

auto map::gm_hide_change(ref_ptr<player> player) -> void {
	if (player->is_using_gm_hide()) {
		update_mob_control(player);
		send(packets::map::remove_player(player->get_id()), player);
	}
	else {
		send(packets::map::player_packet(player), player);
		for (const auto &kvp : m_mobs) {
			if (auto mob = kvp.second) {
				if (mob->get_controller() == nullptr && mob->get_control_status() != mob_control_status::none) {
					update_mob_control(mob);
				}
			}
		}
	}
}

auto map::get_all_player_ids() const -> vector<int32_t> {
	auto copy = m_players;
	vector<game_player_id> ret;
	for (const auto &player : copy) {
		ret.push_back(player->get_id());
	}
	return ret;
}

// Reactors
auto map::add_reactor(reactor *reactor) -> void {
	m_reactors.push_back(reactor);
	reactor->set_id(m_reactors.size() - 1 + map::reactor_start);
}

auto map::get_reactor(size_t id) const -> reactor * {
	return id < m_reactors.size() ? m_reactors[id] : nullptr;
}

auto map::get_num_reactors() const -> size_t {
	return m_reactors.size();
}

auto map::remove_reactor(size_t id) -> void {
	data::type::reactor_spawn_info &info = m_reactor_spawns[id];
	if (info.time >= 0) {
		// We don't want to respawn -1s, leave that to some script
		time_point reactor_respawn = utilities::time::get_now_with_time_added(seconds{info.time});
		m_reactor_respawns.emplace_back(id, reactor_respawn);
	}
}

auto map::kill_reactors(bool show_packet) -> void {
	for (const auto &reactor : m_reactors) {
		if (reactor->is_alive()) {
			reactor->kill();
			if (show_packet) {
				send(packets::destroy_reactor(reactor));
			}
		}
	}
}

// Footholds
auto map::find_floor(const point &pos, point &floor_pos, game_coord start_height_modifier, const rect &search_area) -> search_result {
	// Determines where a drop falls using the footholds data
	//	to check the platforms and find the correct one.
	game_coord x = pos.x;
	game_coord y = pos.y + start_height_modifier;
	game_coord closest_value = std::numeric_limits<game_coord>::max();
	bool any_found = false;
	data::type::foothold_info const * found_foothold = nullptr;

	for (const auto &foothold : m_footholds) {
		const line &line = foothold.line;

		if (line.within_range_x(x)) {
			if (search_area.area() != 0) {
				if (!search_area.contains_any_part_of_line(line)) {
					continue;
				}
			}

			optional<game_coord> y_interpolation = line.interpolate_for_y(x);
			if (y_interpolation.is_initialized()) {
				auto value = y_interpolation.get();
				if (value <= closest_value && value >= y) {
					closest_value = value;
					any_found = true;
					found_foothold = &foothold;
				}
			}
		}
	}

	if (any_found) {
		// We interpolate for X here because otherwise, the X value may not be on the same slope as the foothold
		floor_pos.x = found_foothold->line.interpolate_for_x(closest_value).get(x);
		floor_pos.y = closest_value;
	}

	return any_found ? search_result::found : search_result::not_found;
}

auto map::find_random_floor_pos() -> point {
	point right_bottom = m_real_dimensions.right_bottom();
	point left_top = m_real_dimensions.left_top();
	if (left_top.x == 0) {
		left_top.x = std::numeric_limits<game_coord>::min();
	}
	if (left_top.y == 0) {
		left_top.y = std::numeric_limits<game_coord>::max();
	}
	if (right_bottom.x == 0) {
		right_bottom.x = std::numeric_limits<game_coord>::max();
	}
	if (right_bottom.y == 0) {
		right_bottom.y = std::numeric_limits<game_coord>::min();
	}
	return find_random_floor_pos(rect{left_top, right_bottom});
}

auto map::find_random_floor_pos(const rect &area) -> point {
	vector<const data::type::foothold_info *> valid_footholds;
	rect inside_map_area = area.intersection(m_real_dimensions);
	for (const auto &foothold : m_footholds) {
		// Vertical lines can't be "floors"
		if (!foothold.line.is_vertical() && inside_map_area.contains_any_part_of_line(foothold.line)) {
			valid_footholds.push_back(&foothold);
		}
	}

	point left_top = inside_map_area.left_top();
	point right_bottom = inside_map_area.right_bottom();
	auto x_generate = [&right_bottom, &left_top]() -> game_coord { return randomizer::rand<game_coord>(right_bottom.x, left_top.x); };
	auto y_generate = [&right_bottom, &left_top]() -> game_coord { return randomizer::rand<game_coord>(right_bottom.y, left_top.y); };

	point ret;
	if (valid_footholds.size() == 0) {
		// There's no saving this, just use a random point in the area
		ret.x = x_generate();
		ret.y = y_generate();
		return ret;
	}

	do {
		game_coord x = x_generate();
		game_coord y = y_generate();
		bool any_found = false;
		game_coord closest_value = std::numeric_limits<game_coord>::max();

		for (const auto &foothold : valid_footholds) {
			if (foothold->line.within_range_x(x)) {
				optional<game_coord> y_interpolation = foothold->line.interpolate_for_y(x);
				if (y_interpolation.is_initialized()) {
					auto value = y_interpolation.get();
					if (value <= closest_value && value >= y) {
						closest_value = value;
						any_found = true;
					}
				}
			}
		}

		if (any_found) {
			ret.x = x;
			ret.y = closest_value;
			break;
		}
	} while (true);

	return ret;
}

auto map::get_foothold_at_position(const point &pos) -> game_foothold_id {
	// TODO FIXME
	// Consider refactoring
	game_foothold_id foothold = 0;
	for (const auto &cur : m_footholds) {
		if (cur.line.contains(pos)) {
			foothold = cur.id;
			break;
		}
	}
	return foothold;
}

auto map::is_valid_foothold(game_foothold_id id) -> bool {
	for (const auto &cur : m_footholds) {
		if (cur.id == id) {
			return true;
		}
	}
	return false;
}

auto map::is_vertical_foothold(game_foothold_id id) -> bool {
	for (const auto &cur : m_footholds) {
		if (cur.id == id) {
			return cur.line.is_vertical();
		}
	}
	return false;
}

auto map::get_position_at_foothold(game_foothold_id id) -> point {
	for (const auto &cur : m_footholds) {
		if (cur.id == id) {
			return cur.line.center();
		}
	}
	return point{-1, -1};
}

// Portals
auto map::get_portal(const string &name) const -> const data::type::portal_info * const {
	auto portal = m_portals.find(name);
	return portal != std::end(m_portals) ? &portal->second : nullptr;
}

auto map::get_spawn_point(game_portal_id portal_id) const -> const data::type::portal_info * const {
	game_portal_id id = portal_id != -1 ?
		portal_id :
		randomizer::rand<game_portal_id>(static_cast<game_portal_id>(m_spawn_points.size()) - 1);

	auto iter = m_spawn_points.find(id);
	return &iter->second;
}

auto map::query_portal_name(const string &name, ref_ptr<player> player) const -> const data::type::portal_info * const {
	return name.empty() ?
		nullptr :
		(name == "sp" ?
			get_spawn_point() :
			(name == "tp" ?
				get_mystic_door_portal(player).portal :
				get_portal(name)));
}

auto map::set_portal_state(const string &name, bool enabled) -> void {
	m_portals[name].disabled = !enabled;
}

auto map::get_nearest_spawn_point(const point &pos) const -> const data::type::portal_info * const {
	game_portal_id id = -1;
	int32_t distance = 200000;
	for (const auto &kvp : m_spawn_points) {
		const data::type::portal_info &info = kvp.second;
		int32_t cmp = info.pos - pos;
		if (cmp < distance) {
			id = kvp.first;
			distance = cmp;
		}
	}
	return get_spawn_point(id);
}

auto map::get_portal_names() const -> vector<string> {
	vector<string> ret;
	for (const auto &kvp : m_portals) {
		ret.push_back(kvp.first);
	}
	return ret;
}

auto map::get_town_mystic_door_portal(ref_ptr<player> player) const -> mystic_door_open_result {
	return get_town_mystic_door_portal(player, 0);
}

auto map::get_town_mystic_door_portal(ref_ptr<player> player, uint8_t zero_based_party_index) const -> mystic_door_open_result {
	if (m_info->limitations.mystic_door) {
		return mystic_door_open_result{mystic_door_result::hacking};
	}

	game_map_id town_id = m_info->return_map;
	if (town_id == constant::map::no_map) {
		channel_server::get_instance().log(log_type::hacking, [&](out_stream &str) {
			str << "Likely hacking by player ID " << player->get_id() << ". "
				<< "Mystic Door used on a map that has no return map: " << m_id;
		});
		return mystic_door_open_result{mystic_door_result::hacking};
	}

	map *town = maps::get_map(town_id);
	return town->get_mystic_door_portal(player, zero_based_party_index);
}

auto map::get_mystic_door_portal(ref_ptr<player> player) const -> mystic_door_open_result {
	return get_mystic_door_portal(player, 0);
}

auto map::get_mystic_door_portal(ref_ptr<player> player, uint8_t zero_based_party_index) const -> mystic_door_open_result {
	if (m_door_points.size() == 0) {
		return mystic_door_open_result{mystic_door_result::no_door_points};
	}

	if (m_door_points.size() <= zero_based_party_index) {
		return mystic_door_open_result{mystic_door_result::no_space};
	}

	const data::type::portal_info * const portal = &m_door_points[zero_based_party_index];
	return mystic_door_open_result{get_id(), portal};
}

// NPCs
auto map::remove_npc(size_t index) -> void {
	if (is_valid_npc_index(index)) {
		data::type::npc_spawn_info npc = m_npc_spawns[index];
		game_map_object id = make_npc_id();
		send(packets::npc::show_npc(npc, id, false));
		send(packets::npc::control_npc(npc, id, false));
		m_npc_spawns.erase(std::begin(m_npc_spawns) + index);
	}
}

auto map::add_npc(const data::type::npc_spawn_info &npc) -> game_map_object {
	m_npc_spawns.push_back(npc);
	game_map_object id = make_npc_id();
	send(packets::npc::show_npc(npc, id));
	send(packets::npc::control_npc(npc, id));

	if (channel_server::get_instance().get_npc_data_provider().is_maple_tv(npc.id)) {
		channel_server::get_instance().get_maple_tvs().add_map(this);
	}

	return m_npc_spawns.size() - 1;
}

auto map::is_valid_npc_index(size_t id) const -> bool {
	return id < m_npc_spawns.size();
}

auto map::get_npc(size_t id) const -> data::type::npc_spawn_info {
	return m_npc_spawns[id];
}

// Mobs
auto map::spawn_mob(game_mob_id mob_id, const point &pos, game_foothold_id foothold, ref_ptr<mob> owner, int8_t summon_effect) -> ref_ptr<mob> {
	game_map_object id = m_object_ids.lease();

	auto value = make_ref_ptr<mob>(id, get_id(), mob_id, summon_effect != 0 ? owner : nullptr, pos, -1, false, foothold, mob_control_status::normal);
	if (summon_effect != 0) {
		owner->add_spawn(id, value);
	}

	m_mobs[id] = value;
	send(packets::mobs::spawn_mob(value, summon_effect, owner, (owner == nullptr ? mob_spawn_type::spawn : mob_spawn_type::existing)));
	update_mob_control(value, mob_spawn_type::spawn);

	if (instance *inst = get_instance()) {
		inst->mob_spawn(mob_id, id, get_id());
	}

	return value;
}

auto map::spawn_mob(int32_t spawn_id, const data::type::mob_spawn_info &info) -> ref_ptr<mob> {
	game_map_object id = m_object_ids.lease();

	ref_ptr<mob> no_owner = nullptr;
	auto value = make_ref_ptr<mob>(id, get_id(), info.id, no_owner, info.pos, spawn_id, info.faces_left, info.foothold, mob_control_status::normal);
	m_mobs[id] = value;
	send(packets::mobs::spawn_mob(value, 0, nullptr, mob_spawn_type::spawn));
	update_mob_control(value, mob_spawn_type::spawn);

	if (instance *inst = get_instance()) {
		inst->mob_spawn(info.id, id, get_id());
	}

	return value;
}

auto map::spawn_shell(game_mob_id mob_id, const point &pos, game_foothold_id foothold) -> ref_ptr<mob> {
	game_map_object id = m_object_ids.lease();

	ref_ptr<mob> no_owner = nullptr;
	auto value = make_ref_ptr<mob>(id, get_id(), mob_id, no_owner, pos, -1, false, foothold, mob_control_status::none);
	m_mobs[id] = value;
	update_mob_control(value, mob_spawn_type::spawn);

	if (instance *inst = get_instance()) {
		inst->mob_spawn(mob_id, id, get_id());
	}

	return value;
}

auto map::get_mob(game_map_object map_mob_id) -> ref_ptr<mob> {
	auto kvp = m_mobs.find(map_mob_id);
	return kvp != std::end(m_mobs) ? kvp->second : nullptr;
}

auto map::update_mob_control(ref_ptr<player> player) -> void {
	for (const auto &kvp : m_mobs) {
		if (auto mob = kvp.second) {
			if (mob->get_controller() == player) {
				update_mob_control(mob);
			}
		}
	}
}

auto map::update_mob_control(ref_ptr<mob> mob, mob_spawn_type spawn, ref_ptr<player> display) -> void {
	ref_ptr<player> new_controller = nullptr;
	ref_ptr<player> old_controller = mob->get_controller();
	if (mob->get_control_status() != mob_control_status::none) {
		new_controller = find_controller(mob);
	}
	if (new_controller != old_controller) {
		mob->end_control();
	}
	mob->set_controller(new_controller, spawn, display);
}

auto map::switch_controller(ref_ptr<mob> mob, ref_ptr<player> new_controller) -> void {
	auto old_controller = mob->get_controller();
	mob->end_control();
	mob->set_controller(new_controller, mob_spawn_type::existing, nullptr);
}

auto map::find_controller(ref_ptr<mob> mob) -> ref_ptr<player> {
	int32_t max_pos = 200000;
	ref_ptr<player> controller = nullptr;
	for (const auto &player : m_players) {
		if (!player->is_using_gm_hide()) {
			int32_t cur_pos = mob->get_pos() - player->get_pos();
			if (cur_pos < max_pos) {
				max_pos = cur_pos;
				controller = player;
			}
		}
	}
	return controller;
}

auto map::mob_death(ref_ptr<mob> mob_value, bool from_explosion) -> void {
	auto kvp = m_mobs.find(mob_value->get_map_mob_id());
	if (kvp != std::end(m_mobs)) {
		game_map_object map_mob_id = kvp->first;
		game_mob_id mob_id = mob_value->get_mob_id();
		if (instance *inst = get_instance()) {
			inst->mob_death(mob_id, map_mob_id, m_id);
		}

		if (mob_value->has_status(constant::status_effect::mob::shadow_web)) {
			remove_webbed_mob(map_mob_id);
		}

		if (mob_value->is_sponge()) {
			for (const auto &kvp : mob_value->m_spawns) {
				if (auto spawn = kvp.second.lock()) {
					spawn->kill();
				}
			}
		}
		else {
			switch (mob_id) {
				case constant::mob::zakum_arm1:
				case constant::mob::zakum_arm2:
				case constant::mob::zakum_arm3:
				case constant::mob::zakum_arm4:
				case constant::mob::zakum_arm5:
				case constant::mob::zakum_arm6:
				case constant::mob::zakum_arm7:
				case constant::mob::zakum_arm8:
					if (auto owner = mob_value->m_owner.lock()) {
						if (owner->m_spawns.size() == 1) {
							// Last linked arm is dying
							owner->m_control_status = mob_control_status::normal;
							convert_shell_to_normal(owner);
						}
					}
					break;
			}
		}

		// Spawn death mobs
		if (mob::spawns_sponge(mob_id)) {
			// Special logic to keep units linked
			ref_ptr<mob> sponge = nullptr;
			vector<ref_ptr<mob>> parts;
			for (const auto &summon_id : mob_value->m_info->summon) {
				if (mob::is_sponge(summon_id)) {
					sponge = spawn_mob(summon_id, mob_value->get_pos(), mob_value->get_foothold(), mob_value);
				}
				else {
					parts.push_back(spawn_mob(summon_id, mob_value->get_pos(), mob_value->get_foothold(), mob_value));
				}
			}

			for (const auto &part : parts) {
				part->m_sponge = sponge;
				sponge->add_spawn(part->get_map_mob_id(), part);
			}
		}
		else if (auto sponge = mob_value->get_sponge().lock()) {
			// More special logic to keep units linked
			sponge->m_spawns.erase(map_mob_id);
			for (const auto &summon_id : mob_value->m_info->summon) {
				auto spawn = spawn_mob(summon_id, mob_value->get_pos(), mob_value->get_foothold(), mob_value);
				sponge->add_spawn(map_mob_id, spawn);
			}
		}
		else {
			for (const auto &summon_id : mob_value->m_info->summon) {
				spawn_mob(summon_id, mob_value->get_pos(), mob_value->get_foothold(), mob_value);
			}
		}

		if (mob_value->m_spawns.size() > 0) {
			for (const auto &kvp : mob_value->m_spawns) {
				if (auto spawn = kvp.second.lock()) {
					spawn->m_owner.reset();
				}
			}
		}

		if (auto owner = mob_value->m_owner.lock()) {
			owner->m_spawns.erase(map_mob_id);
		}

		send(packets::mobs::die_mob(map_mob_id, from_explosion ? 4 : 1));
		if (mob_value->m_info->buff != 0) {
			buff_players(mob_value->m_info->buff);
		}

		int32_t spawn_id = mob_value->get_spawn_id();
		if (spawn_id >= 0) {
			data::type::mob_spawn_info &spawn = m_mob_spawns[spawn_id];
			if (spawn.time != -1) {
				// Add spawn point to respawns if mob was spawned by a spawn point
				// Randomly spawn between 1x and 2x the spawn time
				seconds time_modifier = seconds{randomizer::twofold(spawn.time)};
				time_point spawn_time = utilities::time::get_now_with_time_added<seconds>(time_modifier);
				m_mob_respawns.emplace_back(spawn_id, spawn_time);
				spawn.spawned = false;
			}
		}
		m_mobs.erase(kvp);
		m_object_ids.release(map_mob_id);

		if (m_time_mob == map_mob_id) {
			m_time_mob = 0;
		}
	}
}

auto map::mob_summon_skill_used(ref_ptr<mob> mob, const data::type::mob_skill_level_info * const skill) -> void {
	if (m_mobs.size() > 50) {
		return;
	}

	const point &mob_pos = mob->get_pos();
	rect area = skill->dimensions.move(mob_pos).resize(1);

	for (const auto &spawn_id : skill->summons) {
		point floor = find_random_floor_pos(area);
		spawn_mob(spawn_id, floor, 0, mob, skill->summon_effect);
	}
}

auto map::kill_mobs(ref_ptr<player> player, bool distribute_exp_and_drops, game_mob_id mob_id) -> int32_t {
	// Iterator invalidation
	auto mob_map = m_mobs;
	int32_t mobs_killed = 0;
	if (distribute_exp_and_drops) {
		for (const auto &kvp : mob_map) {
			if (auto mob = kvp.second) {
				if (mob_id == 0 || mob->get_mob_id() == mob_id) {
					if (!mob->is_sponge()) {
						// Sponges will be taken care of by their parts
						mob->kill();
					}
					mobs_killed++;
				}
			}
		}
	}
	else {
		for (const auto &kvp : mob_map) {
			if (auto mob = kvp.second) {
				if (mob_id == 0 || mob->get_mob_id() == mob_id) {
					mob_death(mob, false);
					mobs_killed++;
				}
			}
		}
	}
	return mobs_killed;
}

auto map::count_mobs(game_mob_id mob_id) -> int32_t {
	// Iterator invalidation
	auto mob_map = m_mobs;
	int32_t mob_count = 0;
	for (const auto &kvp : mob_map) {
		if (auto mob = kvp.second) {
			if ((mob_id > 0 && mob->get_mob_id() == mob_id) || mob_id == 0) {
				mob_count++;
			}
		}
	}
	return mob_count;
}

auto map::heal_mobs(int32_t base_hp, int32_t heal_range, const rect &dimensions) -> void {
	// Iterator invalidation
	auto mob_map = m_mobs;
	for (const auto &kvp : mob_map) {
		if (auto mob = kvp.second) {
			if (dimensions.contains(mob->get_pos())) {
				mob->skill_heal(base_hp, heal_range);
			}
		}
	}
}

auto map::status_mobs(vector<status_info> &statuses, const rect &dimensions) -> void {
	// Iterator invalidation
	auto mob_map = m_mobs;
	for (const auto &kvp : mob_map) {
		if (auto mob = kvp.second) {
			if (dimensions.contains(mob->get_pos())) {
				mob->add_status(0, statuses);
			}
		}
	}
}

auto map::spawn_zakum(const point &pos, game_foothold_id foothold) -> void {
	auto body = spawn_shell(constant::mob::zakum_body1, pos, foothold);

	init_list<game_mob_id> parts = {
		constant::mob::zakum_arm1, constant::mob::zakum_arm2, constant::mob::zakum_arm3,
		constant::mob::zakum_arm4, constant::mob::zakum_arm5, constant::mob::zakum_arm6,
		constant::mob::zakum_arm7, constant::mob::zakum_arm8
	};

	for (const auto &part : parts) {
		auto spawned_part = spawn_mob(part, pos, foothold);
		spawned_part->set_owner(body);
		body->add_spawn(spawned_part->get_map_mob_id(), spawned_part);
	}
}

auto map::convert_shell_to_normal(ref_ptr<mob> mob) -> void {
	send(packets::mobs::end_control_mob(mob->get_map_mob_id()));
	send(packets::mobs::spawn_mob(mob, 0, nullptr));
	update_mob_control(mob);
}

auto map::add_webbed_mob(game_map_object map_mob_id) -> void {
	m_webbed[map_mob_id] = view_ptr<mob>(m_mobs[map_mob_id]);
}

auto map::remove_webbed_mob(game_map_object map_mob_id) -> void {
	m_webbed.erase(map_mob_id);
}

auto map::run_function_mobs(function<void(ref_ptr<const mob>)> func) -> void {
	for (const auto &kvp : m_mobs) {
		func(kvp.second);
	}
}

// Drops
auto map::add_drop(drop *drop) -> void {
	owned_lock<recursive_mutex> l{m_drops_mutex};
	game_map_object id = m_object_ids.lease();
	drop->set_id(id);
	point found_position = drop->get_pos();
	find_floor(found_position, found_position, -100);
	drop->set_pos(found_position);
	m_drops[id] = drop;
}

auto map::remove_drop(game_map_object id) -> void {
	owned_lock<recursive_mutex> l{m_drops_mutex};
	auto drop = m_drops.find(id);
	if (drop != std::end(m_drops)) {
		m_drops.erase(drop);
		m_object_ids.release(id);
	}
}

auto map::get_drop(game_map_object id) -> drop * {
	owned_lock<recursive_mutex> l{m_drops_mutex};
	auto drop = m_drops.find(id);
	return drop != std::end(m_drops) ? drop->second : nullptr;
}

auto map::clear_drops(bool show_packet) -> void {
	owned_lock<recursive_mutex> l{m_drops_mutex};
	auto copy = m_drops;
	for (const auto &drop : copy) {
		drop.second->remove_drop(show_packet);
	}
}

// Seats
auto map::seat_occupied(game_seat_id id) -> bool {
	auto seat = m_seats.find(id);
	if (seat == std::end(m_seats)) {
		// Hacking
		return true;
	}

	return seat->second.occupant != nullptr;
}

auto map::player_seated(game_seat_id id, ref_ptr<player> player) -> void {
	auto seat = m_seats.find(id);
	if (seat == std::end(m_seats)) {
		// Hacking
		return;
	}

	seat->second.occupant = player.get();
}

// Mists
auto map::add_mist(mist *mist) -> void {
	mist->set_id(m_mist_ids.lease());

	if (mist->is_poison() && !mist->is_mob_mist()) {
		m_poison_mists[mist->get_id()] = mist;
	}
	else {
		m_mists[mist->get_id()] = mist;
	}

	vana::timer::timer::create(
		[this, mist](const time_point &now) { this->remove_mist(mist); },
		vana::timer::id{vana::timer::type::mist_timer, mist->get_id()},
		get_timers(), seconds{mist->get_time()});

	send(packets::map::spawn_mist(mist, false));
}

auto map::get_mist(game_mist_id id) -> mist * {
	auto mist = m_mists.find(id);
	if (mist != std::end(m_mists)) {
		return mist->second;
	}
	mist = m_poison_mists.find(id);
	return mist != std::end(m_poison_mists) ? mist->second : nullptr;
}

auto map::remove_mist(mist *mist) -> void {
	game_mist_id id = mist->get_id();
	if (mist->is_poison() && !mist->is_mob_mist()) {
		m_poison_mists.erase(id);
	}
	else {
		m_mists.erase(id);
	}
	delete mist;
	send(packets::map::remove_mist(id));
	m_mist_ids.release(id);
}

auto map::clear_mists(bool show_packet) -> void {
	auto mistlist = m_mists;
	for (const auto &mist : mistlist) {
		remove_mist(mist.second);
	}
	mistlist = m_poison_mists;
	for (const auto &mist : mistlist) {
		remove_mist(mist.second);
	}
}

// Timer stuff
auto map::respawn(int8_t types) -> void {
	if (types & spawn_types::mob) {
		m_mob_respawns.clear();
		for (size_t spawn_id = 0; spawn_id < m_mob_spawns.size(); spawn_id++) {
			data::type::mob_spawn_info &info = m_mob_spawns[spawn_id];
			if (!info.spawned) {
				info.spawned = true;
				spawn_mob(spawn_id, info);
			}
		}
	}
	if (types & spawn_types::reactor) {
		m_reactor_respawns.clear();
		for (size_t spawn_id = 0; spawn_id < m_reactors.size(); ++spawn_id) {
			reactor *reactor = m_reactors[spawn_id];
			if (!reactor->is_alive()) {
				m_reactor_spawns[spawn_id].spawned = true;
				reactor->restore();
			}
		}
	}
}

auto map::check_spawn(time_point time) -> void {
	if (duration_cast<seconds>(time - m_last_spawn) < seconds{8}) return;

	respawnable *respawn;

	for (size_t i = 0; i < m_mob_respawns.size(); ++i) {
		respawn = &m_mob_respawns[i];
		if (time > respawn->spawn_at) {
			m_mob_spawns[respawn->spawn_id].spawned = true;
			spawn_mob(respawn->spawn_id, m_mob_spawns[respawn->spawn_id]);

			m_mob_respawns.erase(std::begin(m_mob_respawns) + i);
			i--;
		}
	}

	for (size_t i = 0; i < m_reactor_respawns.size(); ++i) {
		respawn = &m_reactor_respawns[i];
		if (time > respawn->spawn_at) {
			m_reactor_spawns[respawn->spawn_id].spawned = true;
			get_reactor(respawn->spawn_id)->restore();

			m_reactor_respawns.erase(std::begin(m_reactor_respawns) + i);
			i--;
		}
	}

	m_last_spawn = time;
}

auto map::check_shadow_web() -> void {
	if (m_webbed.size() > 0) {
		for (const auto &mob : m_webbed) {
			if (auto ptr = mob.second.lock()) {
				ptr->apply_web_damage();
			}
		}
	}
}

auto map::check_mists() -> void {
	if (m_poison_mists.size() == 0) {
		return;
	}

	mist *mist = nullptr;

	for (const auto &kvp : m_mobs) {
		auto mob = kvp.second;

		if (mob == nullptr || mob->has_status(constant::status_effect::mob::poison) || mob->get_hp() == 1) {
			continue;
		}

		for (const auto &kvp : m_poison_mists) {
			mist = kvp.second;
			if (mist->get_area().contains(mob->get_pos())) {
				bool poisoned = mob_handler::handle_mob_status(mist->get_owner_id(), mob, mist->get_skill_id(), mist->get_skill_level(), 0, 0) > 0;
				if (poisoned) {
					// Mob is poisoned, don't need to check any more mists
					break;
				}
			}
		}
	}
}

auto map::clear_drops(time_point time) -> void {
	// Clear drops based on how long they have been in the map
	owned_lock<recursive_mutex> l{m_drops_mutex};

	time -= minutes{3}; // Drops disappear after 3 minutes

	hash_map<game_map_object, drop *> drops = m_drops;
	for (const auto &kvp : drops) {
		if (drop *drop = kvp.second) {
			if (drop->get_dropped_at_time() < time) {
				drop->remove_drop();
			}
		}
	}
}

auto map::map_tick(const time_point &now) -> void {
	if (m_run_unloader) {
		if (s_map_unload_time > 0 && s_map_unload_time > m_max_mob_spawn_time) {
			// TODO FIXME need more robust handling of instances active when the map goes to unload
			if (m_players.size() > 0 || get_instance() != nullptr) {
				m_empty_map_ticks = 0;
			}
			else {
				m_empty_map_ticks++;
				if (m_empty_map_ticks > s_map_unload_time) {
					maps::unload_map(get_id());
					return;
				}
			}
		}
	}

	check_spawn(now);
	clear_drops(now);
	check_mists();

	if (utilities::time::get_second() % 3 == 0) {
		check_shadow_web();
	}
	game_damage dps = m_info->damage_per_second;
	if (dps > 0 && m_players_without_protect_item.size() > 0) {
		for (const auto &kvp : m_players_without_protect_item) {
			if (auto player = kvp.second) {
				if (!player->get_stats()->is_dead() && !player->has_gm_benefits()) {
					player->get_stats()->damage_hp(dps);
				}
			}
		}
	}
}

auto map::check_time_mob_spawn(bool first_load) -> void {
	int32_t c_hour = utilities::time::get_hour(false);
	data::type::time_mob_info tm = m_info->link_info->time_mob.get();

	if (first_load) {
		if (c_hour >= tm.start_hour && c_hour < tm.end_hour) {
			point p = find_random_floor_pos();
			m_time_mob = spawn_mob(tm.id, p, get_foothold_at_position(p), nullptr, 0)->get_map_mob_id();
			send(packets::player::show_message(tm.message, packets::player::notice_types::blue));
		}
	}
	else {
		if (c_hour == tm.start_hour) {
			point p = find_random_floor_pos();
			m_time_mob = spawn_mob(tm.id, p, get_foothold_at_position(p), nullptr, 0)->get_map_mob_id();
			send(packets::player::show_message(tm.message, packets::player::notice_types::blue));
		}
		else if (c_hour == tm.end_hour && m_time_mob != 0) {
			auto m = get_mob(m_time_mob);
			m->kill();
		}
	}
}

auto map::set_map_timer(const seconds &timer) -> void {
	if (timer.count() > 0 && m_timer.count() != 0) {
		throw std::runtime_error{"Timer already executing on map " + utilities::str::lexical_cast<string>(get_id())};
	}

	m_timer = timer;
	m_timer_start = utilities::time::get_now();

	send(packets::map::show_timer(timer));
	if (timer.count() > 0) {
		vana::timer::timer::create(
			[this](const time_point &now) { this->set_map_timer(seconds{0}); },
			vana::timer::id{vana::timer::type::map_timer, get_id(), 25},
			get_timers(), timer);
	}
}

auto map::show_objects(ref_ptr<player> player) -> void {
	// Music
	if (m_music != m_info->default_music) {
		player->send(packets::play_music(m_music));
	}

	// MapleTV messengers
	// TODO FIXME api
	if (channel_server::get_instance().get_maple_tvs().is_maple_tv_map(get_id()) && channel_server::get_instance().get_maple_tvs().has_message()) {
		player->send(packets::maple_tv::show_message(channel_server::get_instance().get_maple_tvs().get_current_message(), channel_server::get_instance().get_maple_tvs().get_message_time()));
	}

	// Players
	for (const auto &map_player : m_players) {
		if (player != map_player && !map_player->is_using_gm_hide()) {
			player->send(packets::map::player_packet(map_player));
			summon_handler::show_summons(map_player, player);
			// Bug in global; would be fixed here:
			// Hurricane/Pierce do not display properly if using when someone enters the map
			// Berserk does not display properly either - players[i]->getActiveBuffs()->getBerserk()
		}
	}

	// NPCs
	int32_t i = 0;
	for (const auto &npc : m_npc_spawns) {
		game_map_object id = i + map::npc_start;
		player->send(packets::npc::show_npc(npc, id));
		player->send(packets::npc::control_npc(npc, id));
		i++;
	}

	// Reactors
	for (const auto &reactor : m_reactors) {
		if (reactor->is_alive()) {
			player->send(packets::spawn_reactor(reactor));
		}
	}

	// Mobs
	for (const auto &kvp : m_mobs) {
		if (auto mob = kvp.second) {
			if (mob->get_control_status() == mob_control_status::none) {
				update_mob_control(mob, mob_spawn_type::spawn, player);
			}
			else {
				player->send(packets::mobs::spawn_mob(mob, 0, nullptr, mob_spawn_type::existing));
				update_mob_control(mob);
			}
		}
	}

	// Drops
	{
		owned_lock<recursive_mutex> l{m_drops_mutex};
		for (const auto &kvp : m_drops) {
			if (drop *drop = kvp.second) {
				drop->show_drop(player);
			}
		}
	}

	// Mists
	for (const auto &kvp : m_mists) {
		if (mist *mist = kvp.second) {
			player->send(packets::map::spawn_mist(mist, true));
		}
	}

	if (party *party = player->get_party()) {
		party->show_hp_bar(player);
		party->receive_hp_bar(player);
	}

	player->get_skills()->on_map_change();

	if (m_info->clock) {
		time_t raw_time = time(nullptr);
		struct tm *time_info = localtime(&raw_time);
		player->send(packets::map::show_clock(time_info->tm_hour, time_info->tm_min, time_info->tm_sec));
	}
}

auto map::send(const packet_builder &builder, ref_ptr<player> sender) -> void {
	for (const auto &map_player : m_players) {
		if (map_player != sender) {
			map_player->send(builder);
		}
	}
}

auto map::send(const split_packet_builder &builder, ref_ptr<player> sender) -> void {
	if (builder.player.get_size() > 0) {
		sender->send(builder.player);
	}

	if (builder.map.get_size() > 0) {
		for (const auto &map_player : m_players) {
			if (map_player != sender) {
				if (!sender->is_using_gm_hide()) {
					map_player->send(builder.map);
				}
			}
		}
	}
}

auto map::create_weather(ref_ptr<player> player, bool admin_weather, int32_t time, int32_t item_id, const string &message) -> bool {
	vana::timer::id timer_id{vana::timer::type::weather_timer}; // Just to check if there's already a weather item running and adding a new one
	if (get_timers()->is_timer_running(timer_id)) {
		// Hacking
		return false;
	}

	send(packets::map::change_weather(admin_weather, item_id, message));
	vana::timer::timer::create(
		[this, admin_weather](const time_point &now) {
			this->send(packets::map::change_weather(admin_weather, 0, ""));
		},
		timer_id,
		get_timers(),
		seconds{time});
	return true;
}

// Instance
auto map::end_instance(bool reset) -> void {
	set_instance(nullptr);
	set_music("default");
	m_mobs.clear();
	for (auto &spawn : m_mob_spawns) {
		spawn.spawned = false;
	}
	clear_drops(false);
	kill_reactors(false);
	if (reset) {
		respawn();
	}
}

}
}