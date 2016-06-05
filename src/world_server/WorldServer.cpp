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
#include "WorldServer.hpp"
#include "common_temp/ConnectionListenerConfig.hpp"
#include "common_temp/ConnectionManager.hpp"
#include "common_temp/ExitCodes.hpp"
#include "common_temp/InitializeCommon.hpp"
#include "common_temp/ServerType.hpp"
#include "common_temp/StringUtilities.hpp"
#include "world_server/Channels.hpp"
#include "world_server/SyncPacket.hpp"
#include "world_server/WorldServerAcceptPacket.hpp"

namespace vana {
namespace world_server {

world_server::world_server() :
	abstract_server{server_type::world}
{
}

auto world_server::shutdown() -> void {
	// If we don't do this and the connection disconnects, it will try to call shutdown() again
	m_world_id = -1;
	abstract_server::shutdown();
}

auto world_server::listen() -> void {
	m_session_pool.initialize(1);

	auto &config = get_inter_server_config();
	get_connection_manager().listen(
		connection_listener_config{
			config.server_ping,
			true,
			connection_type::unknown,
			maple_version::login_subversion,
			m_port,
			ip::type::ipv4
		},
		[&] { return make_ref_ptr<world_server_accepted_session>(*this); }
	);
}

auto world_server::finalize_server_session(ref_ptr<world_server_accepted_session> session) -> void {
	m_session_pool.store(session);
}

auto world_server::load_data() -> result {
	initializing::check_schema_version(this);

	auto &config = get_inter_server_config();
	auto result = get_connection_manager().connect(
		config.login_ip,
		config.login_port,
		config.server_ping,
		server_type::world,
		[&] {
			return make_ref_ptr<login_server_session>();
		});

	if (result.first == result::failure) {
		return result::failure;
	}

	send_auth(result.second);
	return result::successful;
}

auto world_server::on_connect_to_login(ref_ptr<login_server_session> connection) -> void {
	m_login_session = connection;
}

auto world_server::on_disconnect_from_login() -> void {
	m_login_session.reset();

	if (is_connected()) {
		m_world_id = -1;
		log(log_type::server_disconnect, "Disconnected from the LoginServer. Shutting down...");
		get_channels().disconnect();
		exit(exit_code::server_disconnection);
	}
}

auto world_server::rehash_config(const world_config &config) -> void {
	m_config = config;
	m_default_rates = config.rates;
	m_channels.send(packets::interserver::rehash_config(config));
}

auto world_server::established_login_connection(game_world_id world_id, connection_port port, const world_config &conf) -> void {
	m_world_id = world_id;

	log(log_type::server_connect, [&](out_stream &str) {
		str << "Handling world " << static_cast<int32_t>(world_id);
	});

	m_port = port;
	m_config = conf;
	m_default_rates = conf.rates;
	listen();

	m_player_data_provider.load_data();

	display_launch_time();
}

auto world_server::set_rates(const rates_config &rates) -> void {
	m_config.rates = rates;
	m_channels.send(packets::interserver::config::set_rates(rates));
}

auto world_server::reset_rates(int32_t flags) -> void {
	if ((flags & rates_config::types::all) == rates_config::types::all) {
		set_rates(m_default_rates);
	}
	else {
		rates_config new_rates = m_config.rates;
		if ((flags & rates_config::types::mob_exp_rate) == rates_config::types::mob_exp_rate) {
			new_rates.mob_exp_rate = m_default_rates.mob_exp_rate;
		}
		if ((flags & rates_config::types::quest_exp_rate) == rates_config::types::quest_exp_rate) {
			new_rates.quest_exp_rate = m_default_rates.quest_exp_rate;
		}
		if ((flags & rates_config::types::drop_rate) == rates_config::types::drop_rate) {
			new_rates.drop_rate = m_default_rates.drop_rate;
		}
		if ((flags & rates_config::types::drop_meso) == rates_config::types::drop_meso) {
			new_rates.drop_meso = m_default_rates.drop_meso;
		}
		if ((flags & rates_config::types::global_drop_rate) == rates_config::types::global_drop_rate) {
			new_rates.global_drop_rate = m_default_rates.global_drop_rate;
		}
		if ((flags & rates_config::types::global_drop_meso) == rates_config::types::global_drop_meso) {
			new_rates.global_drop_meso = m_default_rates.global_drop_meso;
		}
		set_rates(new_rates);
	}
}

auto world_server::get_player_data_provider() -> player_data_provider & {
	return m_player_data_provider;
}

auto world_server::get_channels() -> channels & {
	return m_channels;
}

auto world_server::make_log_identifier() const -> opt_string {
	return build_log_identifier([&](out_stream &id) { id << "World " << static_cast<int32_t>(m_world_id); });
}

auto world_server::get_log_prefix() const -> string {
	return "world";
}

auto world_server::is_connected() const -> bool {
	return m_world_id != -1;
}

auto world_server::get_world_id() const -> game_world_id {
	return m_world_id;
}

auto world_server::make_channel_port(game_channel_id channel_id) const -> connection_port {
	return m_port + channel_id + 1;
}

auto world_server::get_config() -> const world_config & {
	return m_config;
}

auto world_server::set_scrolling_header(const string &message) -> void {
	m_config.scrolling_header = message;
	m_channels.send(packets::interserver::config::scrolling_header(message));
}

auto world_server::send_login(const packet_builder &builder) -> void {
	m_login_session->send(builder);
}

}
}