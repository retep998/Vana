/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "TimerThread.hpp"
#include "TimeUtilities.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

auto AbstractServer::initialize() -> void {
	m_startTime = TimeUtilities::getNow();

	ConfigFile config("conf/connection_properties.lua");

	m_interPassword = config.get<string_t>("inter_password");
	m_salt = config.get<string_t>("inter_salt");

	if (m_interPassword == "changeme") {
		std::cerr << "ERROR: inter_password is not changed." << std::endl;
		ExitCodes::exit(ExitCodes::ConfigError);
	}
	if (m_salt == "changeme") {
		std::cerr << "ERROR: inter_salt is not changed." << std::endl;
		ExitCodes::exit(ExitCodes::ConfigError);
	}

	m_externalIps = config.getIpMatrix("external_ip");
	m_loginConfig = config.getClass<LoginConfig>();

	loadConfig();
	loadLogConfig();
	loadData();
	if (isListening()) {
		listen();
	}
}

auto AbstractServer::shutdown() -> void {
	ConnectionManager::getInstance().stop();
}

auto AbstractServer::createLogger(const LogConfig &conf) -> void {
	const string_t &timeFormat = conf.timeFormat;
	const string_t &format = conf.format;
	const string_t &file = conf.file;
	int16_t serverType = getServerType();
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
	conf.setVariable("LOG_NONE", LogDestinations::None);
	conf.setVariable("LOG_ALL", LogDestinations::All);

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

		const string_t &newBase = base + "_" + kvp.first;
		val |= kvp.second;
		conf.setVariable(newBase, val);

		if (depth < constants.size()) {
			loggerOptions(constants, conf, newBase, val, depth + 1);
		}
		val = originalVal;
	}
}

auto AbstractServer::log(LogTypes::LogTypes type, const string_t &message) -> void {
	if (Logger *logger = getLogger()) {
		logger->log(type, makeLogIdentifier(), message);
	}
}

auto AbstractServer::displayLaunchTime() const -> void {
	auto loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), getStartTime());
	std::cout << "Started in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}