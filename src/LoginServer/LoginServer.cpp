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
#include "InitializeCommon.h"
#include "InitializeLogin.h"
#include "MapleVersion.h"
#include "RankingCalculator.h"
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
	ConnectionManager::Instance()->accept(m_port, new PlayerFactory(), m_clientEncryption, MapleVersion::PatchLocation);
	ConnectionManager::Instance()->accept(m_interPort, new LoginServerAcceptConnectionFactory(), true);
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
	m_port = config.get<port_t>("port");
	m_interPort = config.get<port_t>("inter_port");
	m_maxInvalidLogins = config.get<int32_t>("invalid_login_threshold");
	m_clientEncryption = config.getBool("use_client_encryption");
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
	MajorBoss boss;
	Configuration conf;
	boost::format formatter("world%i_%s"); // The formatter we'll be using
	size_t i = 0;
	while (true) {
		formatter % i % "name";
		if (!config.keyExists(formatter.str()))
			break; // No more worlds

		World *world = new World();
		conf.name = config.getString(formatter.str());

		formatter % i % "client_encryption";
		conf.clientEncryption = config.getBool(formatter.str());

		formatter % i % "channels";
		conf.maxChannels = config.get<int32_t>(formatter.str());

		formatter % i % "id";
		world->setId(config.get<int8_t>(formatter.str()));

		formatter % i % "ribbon";
		conf.ribbon = config.get<uint8_t>(formatter.str());

		formatter % i % "port";
		world->setPort(config.get<port_t>(formatter.str()));

		formatter % i % "exp_rate";
		conf.expRate = config.get<int32_t>(formatter.str());

		formatter % i % "quest_exp_rate";
		conf.questExpRate = config.get<int32_t>(formatter.str());

		formatter % i % "meso_rate";
		conf.mesoRate = config.get<int32_t>(formatter.str());

		formatter % i % "drop_rate";
		conf.dropRate = config.get<int32_t>(formatter.str());

		formatter % i % "max_stats";
		conf.maxStats = config.get<int16_t>(formatter.str());

		formatter % i % "max_multi_level";
		conf.maxMultiLevel = config.get<uint8_t>(formatter.str());

		formatter % i % "event_message";
		conf.eventMsg = config.getString(formatter.str());

		formatter % i % "scrolling_header";
		conf.scrollingHeader = config.getString(formatter.str());

		formatter % i % "max_player_load";
		conf.maxPlayerLoad = config.get<int32_t>(formatter.str());

		formatter % i % "max_characters";
		conf.maxChars = config.get<int32_t>(formatter.str());

		formatter % i % "fame_time";
		conf.fameTime = config.get<int32_t>(formatter.str());

		formatter % i % "fame_reset_time";
		conf.fameTime = config.get<int32_t>(formatter.str());

		formatter % i % "pianus_attempts";
		boss.attempts = config.get<int16_t>(formatter.str());
		formatter % i % "pianus_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pianus = boss;

		formatter % i % "pap_attempts";
		boss.attempts = config.get<int16_t>(formatter.str());
		formatter % i % "pap_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pap = boss;

		formatter % i % "zakum_attempts";
		boss.attempts = config.get<int16_t>(formatter.str());
		formatter % i % "zakum_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.zakum = boss;

		formatter % i % "horntail_attempts";
		boss.attempts = config.get<int16_t>(formatter.str());
		formatter % i % "horntail_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.horntail = boss;

		formatter % i % "pinkbean_attempts";
		boss.attempts = config.get<int16_t>(formatter.str());
		formatter % i % "pinkbean_channels";
		boss.channels = config.getBossChannels(formatter.str(), conf.maxChannels);
		conf.pinkbean = boss;

		world->setConfiguration(conf);
		Worlds::Instance()->addWorld(world);
		i++;
	}
}