/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "VanaConstants.h"
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
	ConnectionManager::Instance()->accept(m_port, new PlayerFactory(), m_loginConfig, false, MapleVersion::PatchLocation);
	ConnectionManager::Instance()->accept(m_interPort, new LoginServerAcceptConnectionFactory(), m_loginConfig, true);
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
	setListening(true);

	loadWorlds();
}

void LoginServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.getBool("log_login");
	if (enabled) {
		LogConfig log;
		log = conf.getClass<LogConfig>("login");
		createLogger(log);
	}
}

string LoginServer::makeLogIdentifier() {
	// Login needs no special identifier; there's only one
	return "";
}

void LoginServer::loadWorlds() {
	ConfigFile config("conf/worlds.lua");
	WorldConfig conf;

	std::map<int8_t, std::string> recommendedWorlds;

	boost::format formatter("world%i_%s");
	size_t i = 0;
	bool added = false;
	auto getBossConfig = [&formatter, &config, &i](MajorBoss &dest, const string &src, size_t maxChannels) {
		formatter % i % (src + "_attempts");
		dest.attempts = config.get<int16_t>(formatter.str());
		formatter % i % (src + "_channels");
		dest.channels = config.getBossChannels(formatter.str(), maxChannels);
	};

	while (true) {
		formatter % i % "name";
		if (!config.keyExists(formatter.str())) {
			// No more worlds
			break;
		}

		conf.name = config.getString(formatter.str());

		formatter % i % "id";
		int8_t worldId = config.get<int8_t>(formatter.str());

		World *world = Worlds::Instance()->getWorld(worldId);
		added = (world == nullptr);
		if (added) {
			world = new World();
		}

		formatter % i % "channels";
		conf.maxChannels = config.get<int32_t>(formatter.str());

		formatter % i % "ribbon";
		conf.ribbon = config.get<int8_t>(formatter.str());

		formatter % i % "recommended_msg";
		const std::string &msg = config.getString(formatter.str());
		if (!msg.empty()) {
			recommendedWorlds.insert(std::pair<int8_t, std::string>(worldId, msg));
		}

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

		formatter % i % "default_storage";
		conf.defaultStorageSlots = config.get<uint8_t>(formatter.str());

		formatter % i % "fame_time";
		conf.fameTime = config.get<int32_t>(formatter.str());

		formatter % i % "fame_reset_time";
		conf.fameResetTime = config.get<int32_t>(formatter.str());

		getBossConfig(conf.pianus, "pianus", conf.maxChannels);
		getBossConfig(conf.pap, "pap", conf.maxChannels);
		getBossConfig(conf.zakum, "zakum", conf.maxChannels);
		getBossConfig(conf.horntail, "horntail", conf.maxChannels);
		getBossConfig(conf.pinkbean, "pinkbean", conf.maxChannels);

		world->setConfiguration(conf);
		if (added) {
			world->setId(worldId);

			formatter % i % "port";
			world->setPort(config.get<port_t>(formatter.str()));

			Worlds::Instance()->addWorld(world);
		}
		++i;
	}

	Worlds::Instance()->assignRecommendedWorlds(recommendedWorlds);
}