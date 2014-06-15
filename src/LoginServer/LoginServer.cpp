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
#include "LoginServer.hpp"
#include "Configuration.hpp"
#include "ConnectionManager.hpp"
#include "InitializeCommon.hpp"
#include "LoginServerAcceptPacket.hpp"
#include "MapleVersion.hpp"
#include "RankingCalculator.hpp"
#include "UserConnection.hpp"
#include "VanaConstants.hpp"
#include "World.hpp"
#include "Worlds.hpp"
#include <sstream>
#include <iostream>

LoginServer::LoginServer() :
	AbstractServer(ServerType::Login)
{
}

auto LoginServer::listen() -> void {
	auto &config = getInterServerConfig();
	getConnectionManager().accept(Ip::Type::Ipv4, m_port, [] { return new UserConnection(); }, config, false, MapleVersion::LoginSubversion);
	getConnectionManager().accept(Ip::Type::Ipv4, config.loginPort, [] { return new LoginServerAcceptConnection(); }, config, true, MapleVersion::LoginSubversion);
}

auto LoginServer::loadData() -> Result {
	if (Initializing::checkSchemaVersion(true) == Result::Failure) {
		return Result::Failure;
	}
	if (Initializing::checkMcdbVersion() == Result::Failure) {
		return Result::Failure;
	}
	Initializing::setUsersOffline(1);

	m_validCharDataProvider.loadData();
	m_equipDataProvider.loadData();
	m_curseDataProvider.loadData();

	RankingCalculator::setTimer();
	displayLaunchTime();

	return Result::Successful;
}

auto LoginServer::loadConfig() -> Result {
	ConfigFile config("conf/loginserver.lua");
	config.run();
	m_pinEnabled = config.get<bool>("pin");
	m_port = config.get<port_t>("port");
	m_maxInvalidLogins = config.get<int32_t>("invalid_login_threshold");

	loadWorlds();

	return Result::Successful;
}

auto LoginServer::initComplete() -> void {
	listen();
}

auto LoginServer::makeLogIdentifier() const -> opt_string_t {
	// Login needs no special identifier; there's only one
	return opt_string_t();
}

auto LoginServer::getLogPrefix() const -> string_t {
	return "login";
}

auto LoginServer::getPinEnabled() const -> bool {
	return m_pinEnabled;
}

auto LoginServer::rehashConfig() -> void {
	loadWorlds();
	m_worlds.runFunction([](World *world) -> bool {
		if (world != nullptr && world->isConnected()) {
			// We only need to inform worlds that are actually connected
			// Otherwise they'll get the modified config when they connect
			world->send(LoginServerAcceptPacket::rehashConfig(world));
		}
		return false;
	});
}

auto LoginServer::getInvalidLoginThreshold() const -> int32_t {
	return m_maxInvalidLogins;
}

auto LoginServer::getValidCharDataProvider() const -> const ValidCharDataProvider & {
	return m_validCharDataProvider;
}

auto LoginServer::getEquipDataProvider() const -> const EquipDataProvider & {
	return m_equipDataProvider;
}

auto LoginServer::getCurseDataProvider() const -> const CurseDataProvider & {
	return m_curseDataProvider;
}

auto LoginServer::getWorlds() -> Worlds & {
	return m_worlds;
}

auto LoginServer::loadWorlds() -> void {
	ConfigFile config("conf/worlds.lua");
	config.run();
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
	auto getBossConfig = [&getKey, &config](MajorBoss &dest, const string_t &src, channel_id_t maxChannels) {
		dest.attempts = config.get<int16_t>(getKey(src + "_attempts"));

		vector_t<channel_id_t> channels = config.get<vector_t<channel_id_t>>(getKey(src + "_channels"));
		if (channels.size() == 1 && channels[0] == -1) {
			channels.clear();
			for (channel_id_t i = 1; i <= maxChannels; i++) {
				channels.push_back(static_cast<channel_id_t>(i));
			}
		}

		dest.channels = channels;
	};

	while (true) {
		string_t key = getKey("name");
		if (!config.exists(key)) {
			// No more worlds
			break;
		}

		conf.name = config.get<string_t>(key);
		world_id_t worldId = config.get<world_id_t>(getKey("id"));

		World *world = m_worlds.getWorld(worldId);
		added = (world == nullptr);
		if (added) {
			world = new World();
		}

		conf.defaultGmChatMode = config.get<bool>(getKey("gm_chat_by_default"));
		conf.maxChannels = config.get<channel_id_t>(getKey("channels"));
		conf.ribbon = config.get<int8_t>(getKey("ribbon"));
		conf.maxStats = config.get<stat_t>(getKey("max_stats"));
		conf.maxMultiLevel = config.get<player_level_t>(getKey("max_multi_level"));
		conf.eventMsg = config.get<string_t>(getKey("event_message"));
		conf.scrollingHeader = config.get<string_t>(getKey("scrolling_header"));
		conf.maxPlayerLoad = config.get<int32_t>(getKey("max_player_load"));
		conf.maxChars = config.get<int32_t>(getKey("max_characters"));
		conf.defaultChars = config.get<int32_t>(getKey("default_characters"));
		conf.defaultStorageSlots = config.get<storage_slot_t>(getKey("default_storage"));
		conf.fameTime = config.get<seconds_t>(getKey("fame_time"));
		conf.fameResetTime = config.get<seconds_t>(getKey("fame_reset_time"));
		conf.mapUnloadTime = config.get<seconds_t>(getKey("map_unload_time"));

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

			m_worlds.addWorld(world);
		}
		++i;
	}
}