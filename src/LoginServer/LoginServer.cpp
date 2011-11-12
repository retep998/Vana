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
#include "LoginServer.h"
#include "Configuration.h"
#include "ConnectionManager.h"
#include "ExitCodes.h"
#include "InitializeCommon.h"
#include "InitializeLogin.h"
#include "MapleVersion.h"
#include "RankingCalculator.h"
#include "TelnetPlayer.h"
#include "World.h"
#include "Worlds.h"
#include <boost/format.hpp>
#include <iostream>

LoginServer * LoginServer::singleton = nullptr;

LoginServer::LoginServer()
{
	setServerType(ServerTypes::Login);
}

void LoginServer::listen() {
	ConnectionManager::Instance()->accept(m_port, new PlayerFactory(), MapleVersion::PatchLocation);
	ConnectionManager::Instance()->accept(m_interPort, new LoginServerAcceptConnectionFactory());
	if (m_telnetEnabled) {
		ConnectionManager::Instance()->accept(m_telnetPort, new TelnetPlayerFactory());
	}
}

void LoginServer::loadData() {
	Initializing::checkSchemaVersion(true);
	Initializing::checkMcdbVersion();
	Initializing::setUsersOffline(1);
	Initializing::loadData();

	RankingCalculator::setTimer();
	displayLaunchTime();
}

void LoginServer::loadConfig() {
	ConfigFile config("conf/loginserver.lua");
	m_pinEnabled = config.getBool("pin");
	m_port = config.getUnsignedShort("port");
	m_interPort = config.getUnsignedShort("inter_port");
	m_cashPort = config.getUnsignedShort("cash_port");
	m_maxInvalidLogins = config.getInt("invalid_login_threshold");
	m_telnetEnabled = config.getBool("telnet_enabled");
	if (m_telnetEnabled) {
		m_telnetPassword = config.getString("telnet_password");
		m_telnetPort = config.getUnsignedShort("telnet_port");

		if (m_telnetPassword == "changeme") {
			std::cerr << "ERROR: telnet_password is not changed (look inside loginserver.lua)." << std::endl;
			std::cout << "Press enter to quit ...";
			getchar();
			exit(ExitCodes::ConfigError);
		}
	}
	setListening(true);

	loadWorlds();
}

void LoginServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.getBool("log_login");
	if (enabled) {
		LogConfig log = conf.getLogConfig("login");
		createLogger(log);
	}
}

string LoginServer::makeLogIdentifier() {
	return ""; // Login needs no special identifier; there's only one
}

void LoginServer::loadWorlds() {
	ConfigFile config("conf/worlds.lua");
	Configuration conf;
	MajorBoss boss;
	boost::format formatter("world%i_%s"); // The formatter we'll be using
	size_t i = 0;

	while (true) {
		formatter % i % "name";
		if (!config.keyExists(formatter.str()))
			break; // No more worlds

		World *world = new World();
		conf.name = config.getString(formatter.str());

		formatter % i % "channels";
		conf.maxChannels = config.getInt(formatter.str());

		formatter % i % "id";
		world->setId(static_cast<int8_t>(config.getInt(formatter.str())));

		formatter % i % "ribbon";
		conf.ribbon = static_cast<int8_t>(config.getInt(formatter.str()));

		formatter % i % "port";
		world->setPort(static_cast<uint16_t>(config.getShort(formatter.str())));

		formatter % i % "exprate";
		conf.expRate = config.getInt(formatter.str());

		formatter % i % "questexprate";
		conf.questExpRate = config.getInt(formatter.str());

		formatter % i % "mesorate";
		conf.mesoRate = config.getInt(formatter.str());

		formatter % i % "droprate";
		conf.dropRate = config.getInt(formatter.str());

		formatter % i % "maxstats";
		conf.maxStats = config.getShort(formatter.str());

		formatter % i % "max_multi_level";
		conf.maxMultiLevel = static_cast<uint8_t>(config.getInt(formatter.str()));

		formatter % i % "event_msg";
		conf.eventMsg = config.getString(formatter.str());

		formatter % i % "scrolling_header";
		conf.scrollingHeader = config.getString(formatter.str());

		formatter % i % "max_player_load";
		conf.maxPlayerLoad = config.getInt(formatter.str());

		formatter % i % "maxchars";
		conf.maxChars = config.getInt(formatter.str());

		formatter % i % "pianus_attempts";
		boss.attempts = config.getShort(formatter.str());
		formatter % i % "pianus_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pianus = boss;

		formatter % i % "pap_attempts";
		boss.attempts = config.getShort(formatter.str());
		formatter % i % "pap_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pap = boss;

		formatter % i % "zakum_attempts";
		boss.attempts = config.getShort(formatter.str());
		formatter % i % "zakum_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.zakum = boss;

		formatter % i % "horntail_attempts";
		boss.attempts = config.getShort(formatter.str());
		formatter % i % "horntail_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.horntail = boss;

		formatter % i % "pinkbean_attempts";
		boss.attempts = config.getShort(formatter.str());
		formatter % i % "pinkbean_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pinkbean = boss;

		conf.cashPort = m_cashPort + world->getId();

		world->setConfiguration(conf);
		Worlds::Instance()->addWorld(world);
		i++;
	}
}