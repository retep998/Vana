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

LoginServer::LoginServer()
{
	setServerType(ServerTypes::Login);
}

auto LoginServer::listen() -> void {
	ConnectionManager::getInstance().accept(Ip::Type::Ipv4, m_port, new PlayerFactory(), m_loginConfig, false, MapleVersion::PatchLocation);
	ConnectionManager::getInstance().accept(Ip::Type::Ipv4, m_interPort, new LoginServerAcceptConnectionFactory(), m_loginConfig, true);
}

auto LoginServer::loadData() -> void {
	Initializing::checkSchemaVersion(true);
	Initializing::checkMcdbVersion();
	Initializing::setUsersOffline(1);
	Initializing::loadData();

	RankingCalculator::setTimer();
	displayLaunchTime();
}

auto LoginServer::loadConfig() -> void {
	ConfigFile config("conf/loginserver.lua");
	m_pinEnabled = config.get<bool>("pin");
	m_port = config.get<port_t>("port");
	m_interPort = config.get<port_t>("inter_port");
	m_maxInvalidLogins = config.get<int32_t>("invalid_login_threshold");
	setListening(true);

	loadWorlds();
}

auto LoginServer::loadLogConfig() -> void {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.get<bool>("log_login");
	if (enabled) {
		LogConfig log;
		log = conf.getClass<LogConfig>("login");
		createLogger(log);
	}
}

auto LoginServer::makeLogIdentifier() -> opt_string_t {
	// Login needs no special identifier; there's only one
	return opt_string_t();
}

auto LoginServer::rehashConfig() -> void {
	loadWorlds();
	Worlds::getInstance().runFunction([](World *world) -> bool {
		if (world != nullptr && world->isConnected()) {
			// We only need to inform worlds that are actually connected
			// Otherwise they'll get the modified config when they connect
			LoginServerAcceptPacket::rehashConfig(world);
		}
		return false;
	});
}

auto LoginServer::loadWorlds() -> void {
	ConfigFile config("conf/worlds.lua");
	WorldConfig conf;
	out_stream_t stream;
	size_t i = 0;
	bool added = false;
	auto getKey = [&stream, &i](const string_t &key) -> string_t {
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
	auto getBossConfig = [&getKey, &config](MajorBoss &dest, const string_t &src, size_t maxChannels) {
		dest.attempts = config.get<int16_t>(getKey(src + "_attempts"));
		dest.channels = config.getBossChannels(getKey(src + "_channels"), maxChannels);
	};

	while (true) {
		const string_t &key = getKey("name");
		if (!config.keyExists(key)) {
			// No more worlds
			break;
		}

		conf.name = config.get<string_t>(key);
		int8_t worldId = config.get<int8_t>(getKey("id"));

		World *world = Worlds::getInstance().getWorld(worldId);
		added = (world == nullptr);
		if (added) {
			world = new World();
		}

		conf.maxChannels = config.get<int32_t>(getKey("channels"));
		conf.ribbon = config.get<int8_t>(getKey("ribbon"));
		conf.maxStats = config.get<int16_t>(getKey("max_stats"));
		conf.maxMultiLevel = config.get<uint8_t>(getKey("max_multi_level"));
		conf.eventMsg = config.get<string_t>(getKey("event_message"));
		conf.scrollingHeader = config.get<string_t>(getKey("scrolling_header"));
		conf.maxPlayerLoad = config.get<int32_t>(getKey("max_player_load"));
		conf.maxChars = config.get<int32_t>(getKey("max_characters"));
		conf.defaultChars = config.get<int32_t>(getKey("default_characters"));
		conf.defaultStorageSlots = config.get<uint8_t>(getKey("default_storage"));
		conf.fameTime = config.get<int32_t>(getKey("fame_time"));
		conf.fameResetTime = config.get<int32_t>(getKey("fame_reset_time"));
		conf.mapUnloadTime = config.get<int32_t>(getKey("map_unload_time"));

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

			Worlds::getInstance().addWorld(world);
		}
		++i;
	}
}