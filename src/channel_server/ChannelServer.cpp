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
#include "ChannelServer.hpp"
#include "common/ConfigFile.hpp"
#include "common/ConnectionListenerConfig.hpp"
#include "common/ConnectionManager.hpp"
#include "common/ExitCodes.hpp"
#include "common/InitializeCommon.hpp"
#include "common/MiscUtilities.hpp"
#include "common/PacketBuilder.hpp"
#include "common/ServerType.hpp"
#include "channel_server/ChatHandler.hpp"
#include "channel_server/Map.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/ServerPacket.hpp"
#include "channel_server/SyncPacket.hpp"
#include "channel_server/WorldServerPacket.hpp"

namespace vana {
namespace channel_server {

channel_server::channel_server() :
	abstract_server{server_type::channel},
	m_world_ip{0}
{
}

auto channel_server::listen() -> void {
	m_session_pool.initialize(1);

	auto &config = get_inter_server_config();
	get_connection_manager().listen(
		connection_listener_config{
			config.client_ping,
			config.client_encryption,
			connection_type::end_user,
			maple_version::channel_subversion,
			m_port,
			ip::type::ipv4
		},
		[&] { return make_ref_ptr<player>(); }
	);

	initializing::set_users_offline(this, get_online_id());
}

auto channel_server::finalize_player(ref_ptr<player> session) -> void {
	m_session_pool.store(session);
}

auto channel_server::shutdown() -> void {
	// If we don't do this and the connection disconnects, it will try to call shutdown() again
	m_channel_id = -1;
	abstract_server::shutdown();
}

auto channel_server::load_data() -> result {
	if (initializing::check_schema_version(this) == result::failure) {
		return result::failure;
	}
	if (initializing::check_mcdb_version(this) == result::failure) {
		return result::failure;
	}

	m_buff_data_provider.load_data();
	m_valid_char_data_provider.load_data();
	m_equip_data_provider.load_data();
	m_curse_data_provider.load_data();
	m_npc_data_provider.load_data();
	m_drop_data_provider.load_data();
	m_beauty_data_provider.load_data();
	m_mob_data_provider.load_data();
	m_script_data_provider.load_data();
	m_skill_data_provider.load_data();
	m_reactor_data_provider.load_data();
	m_shop_data_provider.load_data();
	m_quest_data_provider.load_data();
	m_item_data_provider.load_data(m_buff_data_provider);
	m_map_data_provider.load_data();
	m_event_data_provider.load_data();

	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Commands... ";
	chat_handler::initialize_commands();
	std::cout << "DONE" << std::endl;

	auto &config = get_inter_server_config();
	auto result = get_connection_manager().connect(
		config.login_ip,
		config.login_port,
		config.server_ping,
		server_type::channel,
		[&] {
			return make_ref_ptr<login_server_session>();
		});

	if (result.first == result::failure) {
		return result::failure;
	}

	send_auth(result.second);
	return result::successful;
}

auto channel_server::reload_data(const string &args) -> void {
	if (args == "all") {
		m_item_data_provider.load_data(m_buff_data_provider);
		m_drop_data_provider.load_data();
		m_shop_data_provider.load_data();
		m_mob_data_provider.load_data();
		m_beauty_data_provider.load_data();
		m_script_data_provider.load_data();
		m_skill_data_provider.load_data();
		m_reactor_data_provider.load_data();
		m_quest_data_provider.load_data();
	}
	else if (args == "items") m_item_data_provider.load_data(m_buff_data_provider);
	else if (args == "drops") m_drop_data_provider.load_data();
	else if (args == "shops") m_shop_data_provider.load_data();
	else if (args == "mobs") m_mob_data_provider.load_data();
	else if (args == "beauty") m_beauty_data_provider.load_data();
	else if (args == "scripts") m_script_data_provider.load_data();
	else if (args == "skills") m_skill_data_provider.load_data();
	else if (args == "reactors") m_reactor_data_provider.load_data();
	else if (args == "quest") m_quest_data_provider.load_data();
}

auto channel_server::make_log_identifier() const -> opt_string {
	return build_log_identifier([&](out_stream &id) {
		id << "World: " << static_cast<int32_t>(m_world_id) << "; ID: " << static_cast<int32_t>(m_channel_id);
	});
}

auto channel_server::get_log_prefix() const -> string {
	return "channel";
}

auto channel_server::connect_to_world(game_world_id world_id, connection_port port, const ip &ip) -> result {
	m_world_id = world_id;
	m_world_port = port;
	m_world_ip = ip;

	auto &config = get_inter_server_config();
	auto result = get_connection_manager().connect(
		ip,
		port,
		config.server_ping,
		server_type::channel,
		[&] {
			return make_ref_ptr<world_server_session>();
		});

	if (result.first == result::failure) {
		return result::failure;
	}

	send_auth(result.second);
	return result::successful;
}

auto channel_server::on_connect_to_login(ref_ptr<login_server_session> session) -> void {
	m_login_connection = session;
}

auto channel_server::on_disconnect_from_login() -> void {
	m_login_connection.reset();
}

auto channel_server::on_connect_to_world(ref_ptr<world_server_session> session) -> void {
	m_world_connection = session;
}

auto channel_server::on_disconnect_from_world() -> void {
	m_world_connection.reset();

	if (is_connected()) {
		log(log_type::server_disconnect, "Disconnected from the WorldServer. Shutting down...");
		m_player_data_provider.disconnect();
		exit(exit_code::server_disconnection);
	}
}

auto channel_server::established_world_connection(game_channel_id channel_id, connection_port port, const world_config &config) -> void {
	m_channel_id = channel_id;

	log(log_type::server_connect, [&](out_stream &str) {
		str << "Handling channel " << static_cast<int32_t>(channel_id) << " on port " << port;
	});

	m_port = port;
	m_config = config;
	map::set_map_unload_time(config.map_unload_time);
	listen();
	display_launch_time();
}

auto channel_server::get_config() const -> const world_config & {
	return m_config;
}

auto channel_server::get_valid_char_data_provider() const -> const valid_char_data_provider & {
	return m_valid_char_data_provider;
}

auto channel_server::get_equip_data_provider() const -> const equip_data_provider & {
	return m_equip_data_provider;
}

auto channel_server::get_curse_data_provider() const -> const curse_data_provider & {
	return m_curse_data_provider;
}

auto channel_server::get_npc_data_provider() const -> const npc_data_provider & {
	return m_npc_data_provider;
}

auto channel_server::get_mob_data_provider() const -> const mob_data_provider & {
	return m_mob_data_provider;
}

auto channel_server::get_beauty_data_provider() const -> const beauty_data_provider & {
	return m_beauty_data_provider;
}

auto channel_server::get_drop_data_provider() const -> const drop_data_provider & {
	return m_drop_data_provider;
}

auto channel_server::get_skill_data_provider() const -> const skill_data_provider & {
	return m_skill_data_provider;
}

auto channel_server::get_shop_data_provider() const -> const shop_data_provider & {
	return m_shop_data_provider;
}

auto channel_server::get_script_data_provider() const -> const script_data_provider & {
	return m_script_data_provider;
}

auto channel_server::get_reactor_data_provider() const -> const reactor_data_provider & {
	return m_reactor_data_provider;
}

auto channel_server::get_item_data_provider() const -> const item_data_provider & {
	return m_item_data_provider;
}

auto channel_server::get_quest_data_provider() const -> const quest_data_provider & {
	return m_quest_data_provider;
}

auto channel_server::get_buff_data_provider() const -> const buff_data_provider & {
	return m_buff_data_provider;
}

auto channel_server::get_event_data_provider() const -> const event_data_provider & {
	return m_event_data_provider;
}

auto channel_server::get_map_data_provider() const -> const map_data_provider & {
	return m_map_data_provider;
}

auto channel_server::get_player_data_provider() -> player_data_provider & {
	return m_player_data_provider;
}

auto channel_server::get_trades() -> trades & {
	return m_trades;
}

auto channel_server::get_maple_tvs() -> maple_tvs & {
	return m_maple_tvs;
}

auto channel_server::get_instances() -> instances & {
	return m_instances;
}

auto channel_server::get_map(int32_t map_id) -> map * {
	return m_map_data_provider.get_map(map_id);
}

auto channel_server::unload_map(int32_t map_id) -> void {
	return m_map_data_provider.unload_map(map_id);
}

auto channel_server::is_connected() const -> bool {
	return m_channel_id != -1;
}

auto channel_server::get_world_id() const -> game_world_id {
	return m_world_id;
}

auto channel_server::get_channel_id() const -> game_channel_id {
	return m_channel_id;
}

auto channel_server::get_online_id() const -> int32_t {
	return 20000 + static_cast<int32_t>(m_world_id) * 100 + m_channel_id;
}

auto channel_server::send_world(const packet_builder &builder) -> void {
	m_world_connection->send(builder);
}

auto channel_server::set_scrolling_header(const string &message) -> void {
	if (m_config.scrolling_header != message) {
		m_config.scrolling_header = message;
		m_player_data_provider.send(packets::show_scrolling_header(message));
	}
}

auto channel_server::modify_rate(int32_t rate_type, int32_t new_value) -> void {
	rates_config current_rates = m_config.rates;
	if (rate_type & rates_config::types::mob_exp_rate) current_rates.mob_exp_rate = new_value;
	if (rate_type & rates_config::types::quest_exp_rate) current_rates.quest_exp_rate = new_value;
	if (rate_type & rates_config::types::drop_rate) current_rates.drop_rate = new_value;
	if (rate_type & rates_config::types::drop_meso) current_rates.drop_meso = new_value;
	if (rate_type & rates_config::types::global_drop_rate) current_rates.global_drop_rate = new_value;
	if (rate_type & rates_config::types::global_drop_meso) current_rates.global_drop_meso = new_value;
	send_world(packets::interserver::config::modify_rates(current_rates));
}

auto channel_server::set_rates(const rates_config &rates) -> void {
	m_config.rates = rates;
}

auto channel_server::set_config(const world_config &config) -> void {
	set_scrolling_header(config.scrolling_header);
	if (config.map_unload_time != m_config.map_unload_time) {
		map::set_map_unload_time(config.map_unload_time);
	}
	m_config = config;
}

}
}