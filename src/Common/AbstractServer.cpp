/*
Copyright (C) 2008-2011 Vana Development Team

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
#include <ctime>
#include <iostream>
#include <iomanip>

AbstractServer::AbstractServer() {
	setListening(false);
}

void AbstractServer::initialize() {
	m_startTime = TimeUtilities::getTickCount();

	ConfigFile config("conf/inter_password.lua");
	m_interPassword = config.getString("inter_password");

	ConfigFile configExtIp("conf/external_ip.lua");
	m_externalIp = configExtIp.getIpMatrix("external_ip");

	if (m_interPassword == "changeme") {
		std::cerr << "ERROR: inter_password is not changed." << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ConfigError);
	}

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
	switch (conf.destination) {
		case LogDestinations::None: m_logger.reset(new Logger("", "", getServerType())); break; // Here's to hoping the compiler optimizes out useless functions
		case LogDestinations::Console: m_logger.reset(new ConsoleLogger(conf.format, conf.timeFormat, getServerType())); break;
		case LogDestinations::File: m_logger.reset(new FileLogger(conf.file, conf.format, conf.timeFormat, getServerType(), conf.bufferSize)); break;
		case LogDestinations::Sql: m_logger.reset(new SqlLogger(conf.format, conf.timeFormat, getServerType(), conf.bufferSize)); break;
	}
}

void AbstractServer::initializeLoggingConstants(ConfigFile &conf) const {
	conf.setVariable("LOG_FILE", LogDestinations::File);
	conf.setVariable("LOG_SQL", LogDestinations::Sql);
	conf.setVariable("LOG_NONE", LogDestinations::None);
	conf.setVariable("LOG_CONSOLE", LogDestinations::Console);
}

void AbstractServer::log(LogTypes::LogTypes type, const string &message) {
	if (Logger *logger = getLogger()) {
		logger->log(type, makeLogIdentifier(), message);
	}
}

void AbstractServer::displayLaunchTime() const {
	float loadingTime = (TimeUtilities::getTickCount() - getStartTime()) / (float) 1000;
	std::cout << "Started in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl << std::endl;
}