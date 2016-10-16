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
#include "login_server.hpp"
#include "common/config/major_boss.hpp"
#include "common/config/rates.hpp"
#include "common/config/salting.hpp"
#include "common/connection_listener_config.hpp"
#include "common/connection_manager.hpp"
#include "common/data/initialize.hpp"
#include "common/maple_version.hpp"
#include "common/server_type.hpp"
#include "login_server/login_server_accept_packet.hpp"
#include "login_server/ranking_calculator.hpp"
#include "login_server/user.hpp"
#include "login_server/world.hpp"
#include "login_server/worlds.hpp"
#include <iostream>
#include <sstream>

namespace vana {
namespace login_server {

login_server::login_server() :
	abstract_server{server_type::login}
{
}

auto login_server::listen() -> void {
	auto &config = get_inter_server_config();

	m_user_pool.initialize(1);
	m_session_pool.initialize(2);

	get_connection_manager().listen(
		connection_listener_config{
			config.client_ping,
			config.client_encryption,
			connection_type::end_user,
			maple_version::login_subversion,
			m_port,
			ip::type::ipv4
		},
		[&] { return make_ref_ptr<user>(); }
	);

	get_connection_manager().listen(
		connection_listener_config{
			config.server_ping,
			true,
			connection_type::unknown,
			maple_version::login_subversion,
			config.login_port,
			ip::type::ipv4
		},
		[&] { return make_ref_ptr<login_server_accepted_session>(*this); }
	);
}

auto login_server::finalize_user(ref_ptr<user> user_value) -> void {
	m_user_pool.store(user_value);
}

auto login_server::finalize_server_session(ref_ptr<login_server_accepted_session> session) -> void {
	m_session_pool.store(session);
}

auto login_server::load_data() -> result {
	if (vana::data::initialize::check_schema_version(this, true) == result::failure) {
		return result::failure;
	}
	if (vana::data::initialize::check_mcdb_version(this) == result::failure) {
		return result::failure;
	}
	vana::data::initialize::set_users_offline(this, 1);

	m_valid_char_data_provider.load_data();
	m_equip_data_provider.load_data();
	m_curse_data_provider.load_data();

	ranking_calculator::set_timer();
	display_launch_time();

	return result::success;
}

auto login_server::load_config() -> result {
	auto config = lua::config_file::get_login_config();
	config->run();
	m_pin_enabled = config->get<bool>("pin");
	m_port = config->get<connection_port>("port");
	m_max_invalid_logins = config->get<int32_t>("invalid_login_threshold");

	auto salting = lua::config_file::get_salting_config();
	salting->run();

	auto salting_conf = salting->get<config::salting>("");
	m_account_salting_policy = salting_conf.account;
	m_account_salt_size = salting_conf.account_salt_size;

	load_worlds();

	return result::success;
}

auto login_server::init_complete() -> void {
	listen();
}

auto login_server::make_log_identifier() const -> opt_string {
	// Login needs no special identifier; there's only one
	return opt_string{};
}

auto login_server::get_log_prefix() const -> string {
	return "login";
}

auto login_server::get_pin_enabled() const -> bool {
	return m_pin_enabled;
}

auto login_server::rehash_config() -> void {
	load_worlds();
	m_worlds.run_function([](world *world_value) -> bool {
		if (world_value != nullptr && world_value->is_connected()) {
			// We only need to inform worlds that are actually connected
			// Otherwise they'll get the modified config when they connect
			world_value->send(packets::interserver::rehash_config(world_value));
		}
		return false;
	});
}

auto login_server::get_invalid_login_threshold() const -> int32_t {
	return m_max_invalid_logins;
}

auto login_server::get_valid_char_data_provider() const -> const data::provider::valid_char & {
	return m_valid_char_data_provider;
}

auto login_server::get_equip_data_provider() const -> const data::provider::equip & {
	return m_equip_data_provider;
}

auto login_server::get_curse_data_provider() const -> const data::provider::curse & {
	return m_curse_data_provider;
}

auto login_server::get_worlds() -> worlds & {
	return m_worlds;
}

auto login_server::get_character_account_salt_size() const -> const config::salt_size & {
	return m_account_salt_size;
}

auto login_server::get_character_account_salting_policy() const -> const config::salt & {
	return m_account_salting_policy;
}

auto login_server::load_worlds() -> void {
	auto config = lua::config_file::get_worlds_config();
	config->run();

	lua::lua_variant worlds_config = config->get<lua::lua_variant>("worlds");
	if (!worlds_config.is(lua::lua_type::table)) {
		config->error("worlds must be a table");
	}

	auto &map = worlds_config.as<ord_map<int32_t, lua::lua_variant>>();
	for (const auto &world_conf : map) {
		auto current_config = world_conf.second.into<config::world>(*config, "worlds." + std::to_string(world_conf.first));

		world *world_value = m_worlds.get_world(current_config.id);
		bool added = (world_value == nullptr);
		if (added) {
			world_value = new world{};
		}

		world_value->set_configuration(current_config);
		if (added) {
			world_value->set_id(current_config.id);
			world_value->set_port(current_config.base_port);
			m_worlds.add_world(world_value);
		}
	}
}

}
}