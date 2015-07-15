/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "AbstractServerConnection.hpp"
#include "ComboLoggers.hpp"
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include "ConnectionManager.hpp"
#include "ConsoleLogger.hpp"
#include "ExitCodes.hpp"
#include "FileLogger.hpp"
#include "Logger.hpp"
#include "MiscUtilities.hpp"
#include "SqlLogger.hpp"
#include "ThreadPool.hpp"
#include "TimerThread.hpp"
#include "TimeUtilities.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

AbstractServer::AbstractServer(ServerType type) :
	m_serverType{type}
{
}

auto AbstractServer::initialize() -> Result {
	m_startTime = TimeUtilities::getNow();

	ConfigFile config("conf/connection_properties.lua");
	config.run();

	m_interPassword = config.get<string_t>("inter_password");
	m_salt = config.get<string_t>("inter_salt");

	if (m_interPassword == "changeme") {
		std::cerr << "ERROR: inter_password is not changed." << std::endl;
		ExitCodes::exit(ExitCodes::ConfigError);
		return Result::Failure;
	}
	if (m_salt == "changeme") {
		std::cerr << "ERROR: inter_salt is not changed." << std::endl;
		ExitCodes::exit(ExitCodes::ConfigError);
		return Result::Failure;
	}

	auto rawIpMap = config.get<vector_t<hash_map_t<string_t, string_t>>>("external_ip");
	for (const auto &pair : rawIpMap) {
		auto ipValue = pair.find("ip");
		auto maskValue = pair.find("mask");
		if (ipValue == std::end(pair) || maskValue == std::end(pair)) {
			std::cerr << "ERROR: External IP configuration is malformed!" << std::endl;
			ExitCodes::exit(ExitCodes::ConfigError);
			return Result::Failure;
		}

		auto ip = Ip::stringToIpv4(ipValue->second);
		auto mask = Ip::stringToIpv4(maskValue->second);
		m_externalIps.push_back(ExternalIp(ip, mask));
	}

	m_interServerConfig = config.get<InterServerConfig>("");

	if (loadConfig() == Result::Failure) {
		return Result::Failure;
	}
	loadLogConfig();

	if (loadData() == Result::Failure) {
		return Result::Failure;
	}
	initComplete();

	m_connectionManager.run();

	return Result::Successful;
}

auto AbstractServer::loadLogConfig() -> void {
	ConfigFile conf("conf/logger.lua");
	initializeLoggingConstants(conf);
	conf.run();

	string_t prefix = getLogPrefix();
	bool enabled = conf.get<bool>("log_" + prefix);
	if (enabled) {
		LogConfig log;
		log = conf.get<LogConfig>(prefix);
		createLogger(log);
	}
}

auto AbstractServer::shutdown() -> void {
	m_connectionManager.stop();
	ThreadPool::wait();
}

auto AbstractServer::getServerType() const -> ServerType {
	return m_serverType;
}

auto AbstractServer::getInterPassword() const -> string_t {
	return MiscUtilities::hashPassword(m_interPassword, m_salt);
}

auto AbstractServer::getInterServerConfig() const -> const InterServerConfig & {
	return m_interServerConfig;
}

auto AbstractServer::loadConfig() -> Result {
	// Intentionally left blank
	return Result::Successful;
}

auto AbstractServer::initComplete() -> void {
	// Intentionally left blank
}

auto AbstractServer::sendAuth(AbstractServerConnection *connection) const -> void {
	connection->sendAuth(getInterPassword(), m_externalIps);
}

auto AbstractServer::createLogger(const LogConfig &conf) -> void {
	const string_t &timeFormat = conf.timeFormat;
	const string_t &format = conf.format;
	const string_t &file = conf.file;
	ServerType serverType = getServerType();
	size_t bufferSize = conf.bufferSize;

	switch (conf.destination) {
		case LogDestinations::Console: m_logger = make_owned_ptr<ConsoleLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::File: m_logger = make_owned_ptr<FileLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::Sql: m_logger = make_owned_ptr<SqlLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileSql: m_logger = make_owned_ptr<DuoLogger<FileLogger, SqlLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileConsole: m_logger = make_owned_ptr<DuoLogger<FileLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::SqlConsole: m_logger = make_owned_ptr<DuoLogger<SqlLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileSqlConsole: m_logger = make_owned_ptr<TriLogger<FileLogger, SqlLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
	}
}

auto AbstractServer::initializeLoggingConstants(ConfigFile &conf) const -> void {
	conf.set<int32_t>("LOG_NONE", LogDestinations::None);
	conf.set<int32_t>("LOG_ALL", LogDestinations::All);

	hash_map_t<string_t, int32_t> constants;
	constants["CONSOLE"] = LogDestinations::Console;
	constants["FILE"] = LogDestinations::File;
	constants["SQL"] = LogDestinations::Sql;
	// If you add more location constants, be sure to add them to this map

	loggerOptions(constants, conf, "LOG", 0, 0);
}

auto AbstractServer::loggerOptions(const hash_map_t<string_t, int32_t> &constants, ConfigFile &conf, const string_t &base, int32_t val, uint32_t depth) const -> void {
	int32_t originalVal = val;
	for (const auto &kvp : constants) {
		if (base.find(kvp.first) != string_t::npos) continue;

		string_t newBase = base + "_" + kvp.first;
		val |= kvp.second;
		conf.set<int32_t>(newBase, val);

		if (depth < constants.size()) {
			loggerOptions(constants, conf, newBase, val, depth + 1);
		}
		val = originalVal;
	}
}

auto AbstractServer::log(LogType type, const string_t &message) -> void {
	if (Logger *logger = m_logger.get()) {
		logger->log(type, makeLogIdentifier(), message);
	}
}

auto AbstractServer::log(LogType type, function_t<void(out_stream_t &)> produceMessage) -> void {
	out_stream_t message;
	produceMessage(message);
	log(type, message.str());
}

auto AbstractServer::log(LogType type, const char *message) -> void {
	log(type, string_t{message});
}

auto AbstractServer::buildLogIdentifier(function_t<void(out_stream_t &)> produceId) const -> opt_string_t {
	out_stream_t message;
	produceId(message);
	return message.str();
}

auto AbstractServer::displayLaunchTime() const -> void {
	auto loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), m_startTime);
	std::cout << "Started in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}