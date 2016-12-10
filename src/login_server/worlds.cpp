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
#include "worlds.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/util/string.hpp"
#include "login_server/channel.hpp"
#include "login_server/characters.hpp"
#include "login_server/login_packet.hpp"
#include "login_server/login_server.hpp"
#include "login_server/login_server_accepted_session.hpp"
#include "login_server/login_server_accept_packet.hpp"
#include "login_server/player_status.hpp"
#include "login_server/user.hpp"
#include "login_server/world.hpp"
#include <iostream>

namespace vana {
namespace login_server {

auto worlds::show_world(ref_ptr<user> user_value) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}

	for (const auto &kvp : m_worlds) {
		if (kvp.second->is_connected()) {
			user_value->send(packets::show_world(kvp.second));
		}
	}
	user_value->send(packets::world_end());
}

auto worlds::add_world(world *world_value) -> void {
	optional<game_world_id> world_id = world_value->get_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}
	m_worlds[world_id.get()] = world_value;
}

auto worlds::select_world(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}

	game_world_id world_id = reader.get<game_world_id>();
	if (world *world_value = get_world(world_id)) {
		int32_t load = world_value->get_player_load();
		int32_t max_load = world_value->get_max_player_load();
		int32_t min_max_load = (max_load / 100) * 90;
		int8_t message = packets::world_messages::normal;

		if (load >= min_max_load && load < max_load) {
			message = packets::world_messages::heavy_load;
		}
		else if (load == max_load) {
			message = packets::world_messages::max_load;
		}
		user_value->send(packets::show_channels(message));
	}
	else {
		// Hacking
		return;
	}
}

auto worlds::channel_select(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}
	game_world_id world_id = reader.get<game_world_id>();
	if (world *world_value = get_world(world_id)) {
		user_value->set_world_id(world_id);
	}
	else {
		// Hacking
		return;
	}

	game_channel_id chan_id = reader.get<int8_t>();

	user_value->send(packets::channel_select());
	world *world_value = m_worlds[world_id];

	if (world_value == nullptr) {
		// Hacking, lag, or client that hasn't been updated (e.g. in the middle of logging in)
		return;
	}

	if (channel *chan = world_value->get_channel(chan_id)) {
		user_value->set_channel(chan_id);
		characters::show_characters(user_value);
	}
	else {
		user_value->send(packets::channel_offline());
	}
}

auto worlds::add_world_server(ref_ptr<login_server_accepted_session> session) -> optional<game_world_id> {
	world *world_value = nullptr;
	for (const auto &kvp : m_worlds) {
		if (!kvp.second->is_connected()) {
			world_value = kvp.second;
			break;
		}
	}

	auto &server = login_server::get_instance();
	if (world_value == nullptr) {
		session->send(packets::interserver::no_more_world());
		server.log(vana::log::type::error, "No more worlds to assign.");
		session->disconnect();
		return {};
	}

	optional<game_world_id> world_id = world_value->get_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	game_world_id cached = world_id.get();
	session->set_world_id(cached);
	world_value->set_connected(true);
	world_value->set_session(session);

	session->send(packets::interserver::connect(world_value));

	server.log(vana::log::type::server_connect, [&](out_stream &log) {
		log << "World " << static_cast<int32_t>(cached);
	});

	return cached;
}

auto worlds::add_channel_server(ref_ptr<login_server_accepted_session> session) -> optional<game_world_id> {
	world *valid_world = nullptr;
	for (const auto &kvp : m_worlds) {
		world *world_value = kvp.second;
		if (world_value->get_channel_count() < world_value->get_max_channels() && world_value->is_connected()) {
			valid_world = world_value;
			break;
		}
	}

	if (valid_world == nullptr) {
		session->send(packets::interserver::connect_channel({}, {}, {}));
		login_server::get_instance().log(vana::log::type::error, "No more channels to assign.");
		session->disconnect();
		return {};
	}

	optional<game_world_id> world_id = valid_world->get_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	ip world_ip = valid_world->match_subnet(session->get_ip().get(ip{0}));
	session->send(packets::interserver::connect_channel(world_id.get(), world_ip, valid_world->get_port()));
	session->disconnect();
	return world_id;
}

auto worlds::send(game_world_id id, const packet_builder &builder) -> void {
	if (world *world_value = get_world(id)) {
		if (world_value->is_connected()) {
			world_value->send(builder);
		}
	}
}

auto worlds::send(const vector<game_world_id> &worlds, const packet_builder &builder) -> void {
	for (const auto &world_id : worlds) {
		auto kvp = m_worlds.find(world_id);
		if (kvp != std::end(m_worlds) && kvp->second->is_connected()) {
			kvp->second->send(builder);
		}
	}
}

auto worlds::send(const packet_builder &builder) -> void {
	for (const auto &kvp : m_worlds) {
		if (kvp.second->is_connected()) {
			kvp.second->send(builder);
		}
	}
}

auto worlds::run_function(function<bool (world *)> func) -> void {
	for (const auto &kvp : m_worlds) {
		if (func(kvp.second)) {
			break;
		}
	}
}

auto worlds::calculate_player_load(world *world_value) -> void {
	world_value->set_player_load(0);
	world_value->run_channel_function([&world_value](channel *chan) {
		world_value->set_player_load(world_value->get_player_load() + chan->get_population());
	});
}

auto worlds::get_world(game_world_id id) -> world * {
	auto kvp = m_worlds.find(id);
	return kvp != std::end(m_worlds) ? kvp->second : nullptr;
}

auto worlds::set_event_messages(const string &message) -> void {
	for (const auto &kvp : m_worlds) {
		kvp.second->set_event_message(message);
	}
}

}
}