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
#pragma once

#include "ConfigFile.hpp"
#include "IPacket.hpp"
#include "Ip.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

struct MajorBoss {
	int16_t attempts = 0;
	vector_t<channel_id_t> channels;
};

template <>
struct PacketSerialize<MajorBoss> {
	auto read(PacketReader &reader) -> MajorBoss {
		MajorBoss ret;
		ret.attempts = reader.get<int16_t>();
		ret.channels = reader.get<vector_t<channel_id_t>>();
		return ret;
	}
	auto write(PacketBuilder &builder, const MajorBoss &obj) -> void {
		builder.add<int16_t>(obj.attempts);
		builder.add<vector_t<channel_id_t>>(obj.channels);
	}
};

struct Rates {
	struct Types {
		static const int32_t MobExpRate = 0x01;
		static const int32_t QuestExpRate = 0x02;
		static const int32_t MobMesoRate = 0x04;
		static const int32_t DropRate = 0x08;
	};

	int32_t mobExpRate = 1;
	int32_t questExpRate = 1;
	int32_t mobMesoRate = 1;
	int32_t dropRate = 1;
};

template <>
struct PacketSerialize<Rates> {
	auto read(PacketReader &reader) -> Rates {
		Rates ret;
		ret.mobExpRate = reader.get<int32_t>();
		ret.questExpRate = reader.get<int32_t>();
		ret.mobMesoRate = reader.get<int32_t>();
		ret.dropRate = reader.get<int32_t>();
		return ret;
	}
	auto write(PacketBuilder &builder, const Rates &obj) -> void {
		builder.add<int32_t>(obj.mobExpRate);
		builder.add<int32_t>(obj.questExpRate);
		builder.add<int32_t>(obj.mobMesoRate);
		builder.add<int32_t>(obj.dropRate);
	}
};

struct WorldConfig {
	bool defaultGmChatMode = true;
	int8_t ribbon = 0;
	player_level_t maxMultiLevel = 1;
	storage_slot_t defaultStorageSlots = 4;
	stat_t maxStats = 999;
	int32_t defaultChars = 3;
	int32_t maxChars = 6;
	int32_t maxPlayerLoad = 100;
	seconds_t fameTime = seconds_t{24 * 60 * 60};
	seconds_t fameResetTime = seconds_t{24 * 60 * 60 * 30};
	seconds_t mapUnloadTime = seconds_t{30 * 60};
	channel_id_t maxChannels = 20;
	string_t eventMsg;
	string_t scrollingHeader;
	string_t name;
	Rates rates;
	MajorBoss pianus;
	MajorBoss pap;
	MajorBoss zakum;
	MajorBoss horntail;
	MajorBoss pinkbean;
};

template <>
struct PacketSerialize<WorldConfig> {
	auto read(PacketReader &reader) -> WorldConfig {
		WorldConfig ret;
		ret.defaultGmChatMode = reader.get<bool>();
		ret.ribbon = reader.get<int8_t>();
		ret.maxMultiLevel = reader.get<player_level_t>();
		ret.defaultStorageSlots = reader.get<storage_slot_t>();
		ret.maxStats = reader.get<stat_t>();
		ret.defaultChars = reader.get<int32_t>();
		ret.maxChars = reader.get<int32_t>();
		ret.maxPlayerLoad = reader.get<int32_t>();
		ret.fameTime = reader.get<seconds_t>();
		ret.fameResetTime = reader.get<seconds_t>();
		ret.mapUnloadTime = reader.get<seconds_t>();
		ret.maxChannels = reader.get<channel_id_t>();
		ret.eventMsg = reader.get<string_t>();
		ret.scrollingHeader = reader.get<string_t>();
		ret.name = reader.get<string_t>();
		ret.rates = reader.get<Rates>();
		ret.pianus = reader.get<MajorBoss>();
		ret.pap = reader.get<MajorBoss>();
		ret.zakum = reader.get<MajorBoss>();
		ret.horntail = reader.get<MajorBoss>();
		ret.pinkbean = reader.get<MajorBoss>();
		return ret;
	}
	auto write(PacketBuilder &builder, const WorldConfig &obj) -> void {
		builder.add<bool>(obj.defaultGmChatMode);
		builder.add<int8_t>(obj.ribbon);
		builder.add<player_level_t>(obj.maxMultiLevel);
		builder.add<storage_slot_t>(obj.defaultStorageSlots);
		builder.add<stat_t>(obj.maxStats);
		builder.add<int32_t>(obj.defaultChars);
		builder.add<int32_t>(obj.maxChars);
		builder.add<int32_t>(obj.maxPlayerLoad);
		builder.add<seconds_t>(obj.fameTime);
		builder.add<seconds_t>(obj.fameResetTime);
		builder.add<seconds_t>(obj.mapUnloadTime);
		builder.add<channel_id_t>(obj.maxChannels);
		builder.add<string_t>(obj.eventMsg);
		builder.add<string_t>(obj.scrollingHeader);
		builder.add<string_t>(obj.name);
		builder.add<Rates>(obj.rates);
		builder.add<MajorBoss>(obj.pianus);
		builder.add<MajorBoss>(obj.pap);
		builder.add<MajorBoss>(obj.zakum);
		builder.add<MajorBoss>(obj.horntail);
		builder.add<MajorBoss>(obj.pinkbean);
	}
};

struct LogConfig {
	int32_t destination = 0;
	uint32_t bufferSize = 20;
	string_t format;
	string_t timeFormat;
	string_t file;
};

template <>
struct LuaSerialize<LogConfig> {
	auto read(LuaEnvironment &config, const string_t &prefix) -> LogConfig {
		LogConfig ret;
		string_t fullPrefix = prefix + "_log_";
		ret.destination = config.get<int32_t>(fullPrefix + "destination");
		ret.bufferSize = config.get<uint32_t>(fullPrefix + "buffer_size");
		ret.format = config.get<string_t>(fullPrefix + "format");
		ret.file = config.get<string_t>(fullPrefix + "file");
		ret.timeFormat = config.get<string_t>("log_time_format");
		return ret;
	}
};

struct DbConfig {
	port_t port = 0;
	string_t database;
	string_t tablePrefix;
	string_t host;
	string_t username;
	string_t password;
};

template <>
struct LuaSerialize<DbConfig> {
	auto read(LuaEnvironment &config, const string_t &prefix) -> DbConfig {
		DbConfig ret;
		case_insensitive_hash_map_t<string_t> obj = config.get<case_insensitive_hash_map_t<string_t>>(prefix);
		auto end = std::end(obj);

		auto kvp = obj.find("database");
		if (kvp == end) config.error("Missing required key 'database' on DB object " + prefix);
		ret.database = kvp->second;

		kvp = obj.find("host");
		if (kvp == end) config.error("Missing required key 'host' on DB object " + prefix);
		ret.host = kvp->second;

		kvp = obj.find("username");
		if (kvp == end) config.error("Missing required key 'username' on DB object " + prefix);
		ret.username = kvp->second;

		kvp = obj.find("port");
		if (kvp == end) config.error("Missing required key 'port' on DB object " + prefix);
		ret.port = StringUtilities::lexical_cast<port_t>(kvp->second);

		kvp = obj.find("password");
		if (kvp != end) ret.password = kvp->second;

		kvp = obj.find("table_prefix");
		if (kvp != end) ret.tablePrefix = kvp->second;

		return ret;
	}
};

struct InterServerConfig {
	InterServerConfig() :
		loginIp(0)
	{
	}

	bool clientEncryption = true;
	bool clientPing = true;
	bool serverPing = true;
	port_t loginPort = 0;
	Ip loginIp;
};

template <>
struct LuaSerialize<InterServerConfig> {
	auto read(LuaEnvironment &config, const string_t &prefix) -> InterServerConfig {
		InterServerConfig ret;
		ret.clientEncryption = config.get<bool>("use_client_encryption");
		ret.clientPing = config.get<bool>("use_client_ping");
		ret.serverPing = config.get<bool>("use_inter_ping");
		ret.loginIp = Ip(Ip::stringToIpv4(config.get<string_t>("login_ip")));
		ret.loginPort = config.get<port_t>("login_inter_port");
		return ret;
	}
};