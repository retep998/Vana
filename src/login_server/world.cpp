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
#include "world.hpp"
#include "common/packet_builder.hpp"
#include "common/util/randomizer.hpp"
#include "login_server/login_server_accepted_session.hpp"

namespace vana {
namespace login_server {

auto world::set_connected(bool connected) -> void {
	m_connected = connected;
	if (!connected) {
		m_session.reset();
	}
}

auto world::set_id(game_world_id id) -> void {
	m_id = id;
}

auto world::set_port(connection_port port) -> void {
	m_port = port;
}

auto world::set_player_load(int32_t load) -> void {
	m_player_load = load;
}

auto world::set_session(ref_ptr<login_server_accepted_session> session) -> void {
	m_session = session;
}

auto world::set_configuration(const config::world &config) -> void {
	m_config = config;
}

auto world::set_event_message(const string &message) -> void {
	m_config.event_message = message;
}

auto world::run_channel_function(function<void (channel *)> func) -> void {
	for (const auto &kvp : m_channels) {
		func(kvp.second.get());
	}
}

auto world::clear_channels() -> void {
	m_channels.clear();
}

auto world::remove_channel(game_channel_id id) -> void {
	m_channels.erase(id);
}

auto world::add_channel(game_channel_id id, channel *chan) -> void {
	m_channels[id].reset(chan);
}

auto world::send(const packet_builder &builder) -> void {
	m_session->send(builder);
}

auto world::is_connected() const -> bool {
	return m_connected;
}

auto world::get_id() const -> optional<game_world_id> {
	return m_id;
}

auto world::get_ribbon() const -> int8_t {
	return m_config.ribbon;
}

auto world::get_port() const -> connection_port {
	return m_port;
}

auto world::get_random_channel() const -> game_channel_id {
	return vana::util::randomizer::select(m_channels)->first;
}

auto world::get_max_channels() const -> game_channel_id {
	return m_config.max_channels;
}

auto world::get_player_load() const -> int32_t {
	return m_player_load;
}

auto world::get_max_player_load() const -> int32_t {
	return m_config.max_player_load;
}

auto world::match_subnet(const ip &test) -> ip {
	return m_session->match_subnet(test);
}

auto world::get_channel_count() const -> game_channel_id {
	return static_cast<game_channel_id>(m_channels.size());
}

auto world::get_name() const -> string {
	return m_config.name;
}

auto world::get_event_message() const -> string {
	return m_config.event_message;
}

auto world::get_channel(game_channel_id id) -> channel * {
	return m_channels.find(id) != std::end(m_channels) ?
		m_channels[id].get() :
		nullptr;
}

auto world::get_config() const -> const config::world & {
	return m_config;
}

}
}