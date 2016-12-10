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
#include "abstract_server.hpp"
#include "common/authentication_packet.hpp"
#include "common/config/log.hpp"
#include "common/config/salting.hpp"
#include "common/connection_manager.hpp"
#include "common/exit_code.hpp"
#include "common/hash_utilities.hpp"
#include "common/log/combo_loggers.hpp"
#include "common/log/console_logger.hpp"
#include "common/log/file_logger.hpp"
#include "common/log/base_logger.hpp"
#include "common/log/sql_logger.hpp"
#include "common/lua/config_file.hpp"
#include "common/session.hpp"
#include "common/timer/thread.hpp"
#include "common/util/misc.hpp"
#include "common/util/thread_pool.hpp"
#include "common/util/time.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace vana {

abstract_server::abstract_server(server_type type) :
	m_server_type{type},
	m_connection_manager{this}
{
}

auto abstract_server::initialize() -> result {
	m_start_time = vana::util::time::get_now();

	load_log_config();

	auto config = lua::config_file::get_connection_properties_config();
	config->run();

	m_inter_password = config->get<string>("inter_password");
	m_salt = config->get<string>("inter_salt");

	if (m_inter_password == "changeme") {
		log(vana::log::type::critical_error, "inter_password is not changed.");
		exit(exit_code::config_error);
		return result::failure;
	}
	if (m_salt == "changeme") {
		log(vana::log::type::critical_error, "inter_salt is not changed.");
		exit(exit_code::config_error);
		return result::failure;
	}

	auto raw_ip_map = config->get<vector<hash_map<string, string>>>("external_ip");
	for (const auto &pair : raw_ip_map) {
		auto ip_value = pair.find("ip");
		auto mask_value = pair.find("mask");
		if (ip_value == std::end(pair) || mask_value == std::end(pair)) {
			log(vana::log::type::critical_error, "External IP configuration is malformed!");
			exit(exit_code::config_error);
			return result::failure;
		}

		auto ip = ip::string_to_ipv4(ip_value->second);
		auto mask = ip::string_to_ipv4(mask_value->second);
		m_external_ips.push_back(external_ip{ip, mask});
	}

	m_inter_server_config = config->get<config::inter_server>("");

	auto salting = lua::config_file::get_salting_config();
	salting->run();

	auto salting_conf = salting->get<config::salting>("");
	m_salting_policy = salting_conf.interserver;

	if (load_config() == result::failure) {
		return result::failure;
	}

	if (load_data() == result::failure) {
		return result::failure;
	}
	init_complete();

	m_connection_manager.run();

	return result::success;
}

auto abstract_server::load_log_config() -> void {
	auto conf = lua::config_file::get_logger_config();
	conf->run();

	string prefix = get_log_prefix();
	config::log log;
	log = conf->get<config::log>(prefix);
	if (log.perform) {
		create_logger(log);
	}
}

auto abstract_server::shutdown() -> void {
	m_connection_manager.stop();
	vana::util::thread_pool::wait();
}

auto abstract_server::get_server_type() const -> server_type {
	return m_server_type;
}

auto abstract_server::get_inter_password() const -> string {
	return hash_utilities::hash_password(m_inter_password, m_salt, m_salting_policy);
}

auto abstract_server::get_interserver_salting_policy() const -> const config::salt & {
	return m_salting_policy;
}

auto abstract_server::get_inter_server_config() const -> const config::inter_server & {
	return m_inter_server_config;
}

auto abstract_server::load_config() -> result {
	// Intentionally left blank
	return result::success;
}

auto abstract_server::init_complete() -> void {
	// Intentionally left blank
}

auto abstract_server::send_auth(ref_ptr<session> session) const -> void {
	session->send(
		packets::send_password(
			vana::util::misc::get_server_type(session->get_type()),
			get_inter_password(),
			m_external_ips));
}

auto abstract_server::create_logger(const config::log &conf) -> void {
	const string &time_format = conf.time_format;
	const string &format = conf.format;
	const string &file = conf.file;
	server_type server_type = get_server_type();
	size_t buffer_size = conf.buffer_size;

	switch (static_cast<vana::log::destination>(conf.destination)) {
		case vana::log::destination::console: m_logger = make_owned_ptr<vana::log::console_logger>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::file: m_logger = make_owned_ptr<vana::log::file_logger>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::sql: m_logger = make_owned_ptr<vana::log::sql_logger>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::file_sql: m_logger = make_owned_ptr<vana::log::dual_logger<vana::log::file_logger, vana::log::sql_logger>>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::file_console: m_logger = make_owned_ptr<vana::log::dual_logger<vana::log::file_logger, vana::log::console_logger>>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::sql_console: m_logger = make_owned_ptr<vana::log::dual_logger<vana::log::sql_logger, vana::log::console_logger>>(file, format, time_format, server_type, buffer_size); break;
		case vana::log::destination::file_sql_console: m_logger = make_owned_ptr<vana::log::tri_logger<vana::log::file_logger, vana::log::sql_logger, vana::log::console_logger>>(file, format, time_format, server_type, buffer_size); break;
	}
}

auto abstract_server::log(vana::log::type type, const string &message) -> void {
	if (auto logger = m_logger.get()) {
		logger->log(type, make_log_identifier(), message);
	}
}

auto abstract_server::log(vana::log::type type, function<void(out_stream &)> produce_message) -> void {
	out_stream message;
	produce_message(message);
	log(type, message.str());
}

auto abstract_server::log(vana::log::type type, const char *message) -> void {
	log(type, string{message});
}

auto abstract_server::build_log_identifier(function<void(out_stream &)> produce_id) const -> opt_string {
	out_stream message;
	produce_id(message);
	return message.str();
}

auto abstract_server::display_launch_time() const -> void {
	auto loading_time = vana::util::time::get_distance<milliseconds>(vana::util::time::get_now(), m_start_time);
	std::cout << "Started in " << std::setprecision(3) << loading_time / 1000.f << " seconds!" << std::endl << std::endl;
}

}