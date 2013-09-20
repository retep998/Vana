/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "AbstractServer.h"
#include "ComboLoggers.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "ConnectionManager.h"
#include "ConsoleLogger.h"
#include "ExitCodes.h"
#include "FileLogger.h"
#include "Logger.h"
#include "MiscUtilities.h"
#include "SqlLogger.h"
#include "TimeUtilities.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

AbstractServer::AbstractServer()
{
	setListening(false);
}

void AbstractServer::initialize() {
	m_startTime = TimeUtilities::getNow();

	ConfigFile config("conf/connection_properties.lua");

	m_interPassword = config.getString("inter_password");
	m_salt = config.getString("inter_salt");

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

void AbstractServer::shutdown() {
	ConnectionManager::Instance()->stop();
	m_logger.reset(nullptr);
}

void AbstractServer::createLogger(const LogConfig &conf) {
	const string &tForm = conf.timeFormat;
	const string &form = conf.format;
	const string &file = conf.file;
	int16_t sType = getServerType();
	size_t bSize = conf.bufferSize;

	switch (conf.destination) {
		case LogDestinations::Console: m_logger.reset(new ConsoleLogger(file, form, tForm, sType, bSize)); break;
		case LogDestinations::File: m_logger.reset(new FileLogger(file, form, tForm, sType, bSize)); break;
		case LogDestinations::Sql: m_logger.reset(new SqlLogger(file, form, tForm, sType, bSize)); break;
		case LogDestinations::FileSql: m_logger.reset(new DuoLogger<FileLogger, SqlLogger>(file, form, tForm, sType, bSize)); break;
		case LogDestinations::FileConsole: m_logger.reset(new DuoLogger<FileLogger, ConsoleLogger>(file, form, tForm, sType, bSize)); break;
		case LogDestinations::SqlConsole: m_logger.reset(new DuoLogger<SqlLogger, ConsoleLogger>(file, form, tForm, sType, bSize)); break;
		case LogDestinations::FileSqlConsole: m_logger.reset(new TriLogger<FileLogger, SqlLogger, ConsoleLogger>(file, form, tForm, sType, bSize)); break;
	}
}

void AbstractServer::initializeLoggingConstants(ConfigFile &conf) const {
	conf.setVariable("LOG_NONE", LogDestinations::None);
	conf.setVariable("LOG_ALL", LogDestinations::All);

	ConstantMap constants;
	constants["CONSOLE"] = LogDestinations::Console;
	constants["FILE"] = LogDestinations::File;
	constants["SQL"] = LogDestinations::Sql;
	// If you add more location constants, be sure to add them to this map

	loggerOptions(constants, conf, "LOG", 0, 0);
}

void AbstractServer::loggerOptions(const ConstantMap &constants, ConfigFile &conf, const string &base, int32_t val, uint32_t depth) const {
	int32_t oVal = val;
	for (ConstantMap::const_iterator iter = constants.begin(); iter != constants.end(); ++iter) {
		if (base.find(iter->first) != string::npos) continue;

		const string &newBase = base + "_" + iter->first;
		val |= iter->second;
		conf.setVariable(newBase, val);

		if (depth < constants.size()) {
			loggerOptions(constants, conf, newBase, val, depth + 1);
		}
		val = oVal;
	}
}

void AbstractServer::log(LogTypes::LogTypes type, const string &message) {
	if (Logger *logger = getLogger()) {
		logger->log(type, makeLogIdentifier(), message);
	}
}

void AbstractServer::displayLaunchTime() const {
	milliseconds_t::rep loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), getStartTime());
	std::cout << "Started in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}