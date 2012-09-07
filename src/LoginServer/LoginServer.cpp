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
#include "LoginServerAcceptPacket.h"
#include "MapleVersion.h"
#include "RankingCalculator.h"
#include "VanaConstants.h"
#include "World.h"
#include "Worlds.h"
#include <sstream>
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

opt_string LoginServer::makeLogIdentifier() {
	// Login needs no special identifier; there's only one
	return opt_string();
}

void LoginServer::rehashConfig() {
	loadWorlds();
	Worlds::Instance()->runFunction([](World *world) -> bool {
		if (world != nullptr && world->getConnection() != nullptr) {
			// We only need to inform worlds that are actually connected
			// Otherwise they'll get the modified config when they connect
			LoginServerAcceptPacket::rehashConfig(world);
		}
		return false;
	});
}

void LoginServer::loadWorlds() {
	ConfigFile config("conf/worlds.lua");
	WorldConfig conf;
	std::ostringstream stream;
	size_t i = 0;
	bool added = false;
	auto getKey = [&stream, &i](const string &key) -> string {
		stream.clear();
		stream.str("");
		stream << "world" << i << "_" << key;
		return stream.str();
	};
	auto getRatesConfig = [&getKey, &config](Rates &dest) {
		dest.mobExpRate = config.get<int32_t>(getKey("mob_exp_rate"));
		dest.questExpRate = config.get<int32_t>(getKey("quest_exp_rate"));
		dest.mobMesoRate = config.get<int32_t>(getKey("mob_meso_rate"));
		dest.dropRate = config.get<int32_t>(getKey("drop_rate"));
	};
	auto getBossConfig = [&getKey, &config](MajorBoss &dest, const string &src, size_t maxChannels) {
		dest.attempts = config.get<int16_t>(getKey(src + "_attempts"));
		dest.channels = config.getBossChannels(getKey(src + "_channels"), maxChannels);
	};

	while (true) {
		const string &key = getKey("name");
		if (!config.keyExists(key)) {
			// No more worlds
			break;
		}

		conf.name = config.getString(key);
		int8_t worldId = config.get<int8_t>(getKey("id"));

		World *world = Worlds::Instance()->getWorld(worldId);
		added = (world == nullptr);
		if (added) {
			world = new World();
		}

		conf.maxChannels = config.get<int32_t>(getKey("channels"));
		conf.ribbon = config.get<int8_t>(getKey("ribbon"));
		conf.maxStats = config.get<int16_t>(getKey("max_stats"));
		conf.maxMultiLevel = config.get<uint8_t>(getKey("max_multi_level"));
		conf.eventMsg = config.getString(getKey("event_message"));
		conf.scrollingHeader = config.getString(getKey("scrolling_header"));
		conf.maxPlayerLoad = config.get<int32_t>(getKey("max_player_load"));
		conf.maxChars = config.get<int32_t>(getKey("max_characters"));
		conf.defaultChars = config.get<int32_t>(getKey("default_characters"));
		conf.defaultStorageSlots = config.get<uint8_t>(getKey("default_storage"));
		conf.fameTime = config.get<int32_t>(getKey("fame_time"));
		conf.fameResetTime = config.get<int32_t>(getKey("fame_reset_time"));

		getRatesConfig(conf.rates);
		getBossConfig(conf.pianus, "pianus", conf.maxChannels);
		getBossConfig(conf.pap, "pap", conf.maxChannels);
		getBossConfig(conf.zakum, "zakum", conf.maxChannels);
		getBossConfig(conf.horntail, "horntail", conf.maxChannels);
		getBossConfig(conf.pinkbean, "pinkbean", conf.maxChannels);

		world->setConfiguration(conf);
		if (added) {
			world->setId(worldId);
			world->setPort(config.get<port_t>(getKey("port")));

			Worlds::Instance()->addWorld(world);
		}
		++i;
	}
}