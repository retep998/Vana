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
#pragma once

#include "ConfigFile.hpp"
#include "IConfig.hpp"
#include "IPacket.hpp"
#include "Ip.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

struct MajorBoss : public IPacketSerializable {
	auto write(PacketBuilder &builder) const -> void override {
		builder.add<int16_t>(attempts);
		builder.addVector(channels);
	}

	auto read(PacketReader &reader) -> void override {
		attempts = reader.get<int16_t>();
		channels = reader.getVector<channel_id_t>();
	}

	int16_t attempts = 0;
	vector_t<channel_id_t> channels;
};

struct Rates : public IPacketSerializable {
	struct Types {
		static const int32_t MobExpRate = 0x01;
		static const int32_t QuestExpRate = 0x02;
		static const int32_t MobMesoRate = 0x04;
		static const int32_t DropRate = 0x08;
	};

	auto write(PacketBuilder &builder) const -> void override {
		builder.add<int32_t>(mobExpRate);
		builder.add<int32_t>(questExpRate);
		builder.add<int32_t>(mobMesoRate);
		builder.add<int32_t>(dropRate);
	}

	auto read(PacketReader &reader) -> void override {
		mobExpRate = reader.get<int32_t>();
		questExpRate = reader.get<int32_t>();
		mobMesoRate = reader.get<int32_t>();
		dropRate = reader.get<int32_t>();
	}

	int32_t mobExpRate = 1;
	int32_t questExpRate = 1;
	int32_t mobMesoRate = 1;
	int32_t dropRate = 1;
};

struct WorldConfig : public IPacketSerializable {
	auto write(PacketBuilder &builder) const -> void override {
		builder.add<int8_t>(ribbon);
		builder.add<uint8_t>(maxMultiLevel);
		builder.add<uint8_t>(defaultStorageSlots);
		builder.add<int16_t>(maxStats);
		builder.add<int32_t>(defaultChars);
		builder.add<int32_t>(maxChars);
		builder.add<int32_t>(maxPlayerLoad);
		builder.add<int32_t>(fameTime);
		builder.add<int32_t>(fameResetTime);
		builder.add<int32_t>(mapUnloadTime);
		builder.add<channel_id_t>(maxChannels);
		builder.addString(eventMsg);
		builder.addString(scrollingHeader);
		builder.addString(name);
		builder.addClass<Rates>(rates);
		builder.addClass<MajorBoss>(pianus);
		builder.addClass<MajorBoss>(pap);
		builder.addClass<MajorBoss>(zakum);
		builder.addClass<MajorBoss>(horntail);
		builder.addClass<MajorBoss>(pinkbean);
	}

	auto read(PacketReader &reader) -> void override {
		ribbon = reader.get<int8_t>();
		maxMultiLevel = reader.get<uint8_t>();
		defaultStorageSlots = reader.get<uint8_t>();
		maxStats = reader.get<int16_t>();
		defaultChars = reader.get<int32_t>();
		maxChars = reader.get<int32_t>();
		maxPlayerLoad = reader.get<int32_t>();
		fameTime = reader.get<int32_t>();
		fameResetTime = reader.get<int32_t>();
		mapUnloadTime = reader.get<int32_t>();
		maxChannels = reader.get<channel_id_t>();
		eventMsg = reader.getString();
		scrollingHeader = reader.getString();
		name = reader.getString();
		rates = reader.getClass<Rates>();
		pianus = reader.getClass<MajorBoss>();
		pap = reader.getClass<MajorBoss>();
		zakum = reader.getClass<MajorBoss>();
		horntail = reader.getClass<MajorBoss>();
		pinkbean = reader.getClass<MajorBoss>();
	}

	int8_t ribbon = 0;
	uint8_t maxMultiLevel = 1;
	uint8_t defaultStorageSlots = 4;
	int16_t maxStats = 999;
	int32_t defaultChars = 3;
	int32_t maxChars = 6;
	int32_t maxPlayerLoad = 100;
	int32_t fameTime = 30 * 60 * 60;
	int32_t fameResetTime = 30 * 60 * 60;
	int32_t mapUnloadTime = 30 * 60;
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

struct LogConfig : public IConfigReadable {
	auto read(ConfigFile &conf, const string_t &prefix) -> void override {
		const string_t &fullPrefix = prefix + "_log_";
		destination = conf.get<int32_t>(fullPrefix + "destination");
		bufferSize = conf.get<uint32_t>(fullPrefix + "buffer_size");
		format = conf.get<string_t>(fullPrefix + "format");
		file = conf.get<string_t>(fullPrefix + "file");
		timeFormat = conf.get<string_t>("log_time_format");
	}

	int32_t destination = 0;
	uint32_t bufferSize = 20;
	string_t format;
	string_t timeFormat;
	string_t file;
};

struct DbConfig : public IConfigReadable {
	auto read(ConfigFile &conf, const string_t &prefix) -> void override {
		const string_t &fullPrefix = prefix + "_";
		database = conf.get<string_t>(fullPrefix + "database");
		host = conf.get<string_t>(fullPrefix + "host");
		username = conf.get<string_t>(fullPrefix + "username");
		password = conf.get<string_t>(fullPrefix + "password");
		port = conf.get<port_t>(fullPrefix + "port");
	}

	port_t port = 0;
	string_t database;
	string_t host;
	string_t username;
	string_t password;
};

struct InterServerConfig : public IConfigReadable {
	InterServerConfig() :
		loginIp(0)
	{
	}

	auto read(ConfigFile &conf, const string_t &prefix) -> void override {
		clientEncryption = conf.get<bool>("use_client_encryption");
		clientPing = conf.get<bool>("use_client_ping");
		serverPing = conf.get<bool>("use_inter_ping");
		loginIp = Ip(Ip::stringToIpv4(conf.get<string_t>("login_ip")));
		loginPort = conf.get<port_t>("login_inter_port");
	}

	bool clientEncryption = true;
	bool clientPing = true;
	bool serverPing = true;
	port_t loginPort = 0;
	Ip loginIp;
};