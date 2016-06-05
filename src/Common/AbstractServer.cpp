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
#include "AbstractServer.hpp"
#include "Common/AuthenticationPacket.hpp"
#include "Common/ComboLoggers.hpp"
#include "Common/ConfigFile.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/ConsoleLogger.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/FileLogger.hpp"
#include "Common/HashUtilities.hpp"
#include "Common/LogConfig.hpp"
#include "Common/Logger.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/SaltingConfig.hpp"
#include "Common/Session.hpp"
#include "Common/SqlLogger.hpp"
#include "Common/ThreadPool.hpp"
#include "Common/TimerThread.hpp"
#include "Common/TimeUtilities.hpp"
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
	m_start_time = utilities::time::get_now();

	load_log_config();

	auto config = config_file::get_connection_properties_config();
	config->run();

	m_inter_password = config->get<string>("inter_password");
	m_salt = config->get<string>("inter_salt");

	if (m_inter_password == "changeme") {
		log(log_type::critical_error, "inter_password is not changed.");
		exit(exit_code::config_error);
		return result::failure;
	}
	if (m_salt == "changeme") {
		log(log_type::critical_error, "inter_salt is not changed.");
		exit(exit_code::config_error);
		return result::failure;
	}

	auto raw_ip_map = config->get<vector<hash_map<string, string>>>("external_ip");
	for (const auto &pair : raw_ip_map) {
		auto ip_value = pair.find("ip");
		auto mask_value = pair.find("mask");
		if (ip_value == std::end(pair) || mask_value == std::end(pair)) {
			log(log_type::critical_error, "External IP configuration is malformed!");
			exit(exit_code::config_error);
			return result::failure;
		}

		auto ip = ip::string_to_ipv4(ip_value->second);
		auto mask = ip::string_to_ipv4(mask_value->second);
		m_external_ips.push_back(external_ip{ip, mask});
	}

	m_inter_server_config = config->get<inter_server_config>("");

	auto salting = config_file::get_salting_config();
	salting->run();

	auto salting_conf = salting->get<salting_config>("");
	m_salting_policy = salting_conf.interserver;

	if (load_config() == result::failure) {
		return result::failure;
	}

	if (load_data() == result::failure) {
		return result::failure;
	}
	init_complete();

	m_connection_manager.run();

	return result::successful;
}

auto abstract_server::load_log_config() -> void {
	auto conf = config_file::get_logger_config();
	conf->run();

	string prefix = get_log_prefix();
	log_config log;
	log = conf->get<log_config>(prefix);
	if (log.log) {
		create_logger(log);
	}
}

auto abstract_server::shutdown() -> void {
	m_connection_manager.stop();
	thread_pool::wait();
}

auto abstract_server::get_server_type() const -> server_type {
	return m_server_type;
}

auto abstract_server::get_inter_password() const -> string {
	return hash_utilities::hash_password(m_inter_password, m_salt, m_salting_policy);
}

auto abstract_server::get_interserver_salting_policy() const -> const salt_config & {
	return m_salting_policy;
}

auto abstract_server::get_inter_server_config() const -> const inter_server_config & {
	return m_inter_server_config;
}

auto abstract_server::load_config() -> result {
	// Intentionally left blank
	return result::successful;
}

auto abstract_server::init_complete() -> void {
	// Intentionally left blank
}

auto abstract_server::send_auth(ref_ptr<session> session) const -> void {
	session->send(
		packets::send_password(
			utilities::misc::get_server_type(session->get_type()),
			get_inter_password(),
			m_external_ips));
}

auto abstract_server::create_logger(const log_config &conf) -> void {
	const string &time_format = conf.time_format;
	const string &format = conf.format;
	const string &file = conf.file;
	server_type server_type = get_server_type();
	size_t buffer_size = conf.buffer_size;

	switch (conf.destination) {
		case log_destinations::console: m_logger = make_owned_ptr<console_logger>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::file: m_logger = make_owned_ptr<file_logger>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::sql: m_logger = make_owned_ptr<sql_logger>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::file_sql: m_logger = make_owned_ptr<duo_logger<file_logger, sql_logger>>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::file_console: m_logger = make_owned_ptr<duo_logger<file_logger, console_logger>>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::sql_console: m_logger = make_owned_ptr<duo_logger<sql_logger, console_logger>>(file, format, time_format, server_type, buffer_size); break;
		case log_destinations::file_sql_console: m_logger = make_owned_ptr<tri_logger<file_logger, sql_logger, console_logger>>(file, format, time_format, server_type, buffer_size); break;
	}
}

auto abstract_server::log(log_type type, const string &message) -> void {
	if (auto logger = m_logger.get()) {
		logger->log(type, make_log_identifier(), message);
	}
}

auto abstract_server::log(log_type type, function<void(out_stream &)> produce_message) -> void {
	out_stream message;
	produce_message(message);
	log(type, message.str());
}

auto abstract_server::log(log_type type, const char *message) -> void {
	log(type, string{message});
}

auto abstract_server::build_log_identifier(function<void(out_stream &)> produce_id) const -> opt_string {
	out_stream message;
	produce_id(message);
	return message.str();
}

auto abstract_server::display_launch_time() const -> void {
	auto loading_time = utilities::time::get_distance<milliseconds>(utilities::time::get_now(), m_start_time);
	std::cout << "Started in " << std::setprecision(3) << loading_time / 1000.f << " seconds!" << std::endl << std::endl;
}

}