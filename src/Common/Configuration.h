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

#include "ConfigFile.h"
#include "IConfig.h"
#include "IPacket.h"
#include "Ip.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Types.h"
#include <string>
#include <vector>

struct MajorBoss : public IPacketSerializable {
	auto write(PacketCreator &packet) const -> void override {
		packet.add<int16_t>(attempts);
		packet.addVector(channels);
	}

	auto read(PacketReader &packet) -> void override {
		attempts = packet.get<int16_t>();
		channels = packet.getVector<int8_t>();
	}

	int16_t attempts = 0;
	vector_t<int8_t> channels;
};

struct Rates : public IPacketSerializable {
	struct Types {
		static const int32_t MobExpRate = 0x01;
		static const int32_t QuestExpRate = 0x02;
		static const int32_t MobMesoRate = 0x04;
		static const int32_t DropRate = 0x08;
	};

	auto write(PacketCreator &packet) const -> void override {
		packet.add<int32_t>(mobExpRate);
		packet.add<int32_t>(questExpRate);
		packet.add<int32_t>(mobMesoRate);
		packet.add<int32_t>(dropRate);
	}

	auto read(PacketReader &packet) -> void override {
		mobExpRate = packet.get<int32_t>();
		questExpRate = packet.get<int32_t>();
		mobMesoRate = packet.get<int32_t>();
		dropRate = packet.get<int32_t>();
	}

	int32_t mobExpRate = 1;
	int32_t questExpRate = 1;
	int32_t mobMesoRate = 1;
	int32_t dropRate = 1;
};

struct WorldConfig : public IPacketSerializable {
	auto write(PacketCreator &packet) const -> void override {
		packet.add<int8_t>(ribbon);
		packet.add<uint8_t>(maxMultiLevel);
		packet.add<uint8_t>(defaultStorageSlots);
		packet.add<int16_t>(maxStats);
		packet.add<int32_t>(defaultChars);
		packet.add<int32_t>(maxChars);
		packet.add<int32_t>(maxPlayerLoad);
		packet.add<int32_t>(maxChannels);
		packet.add<int32_t>(fameTime);
		packet.add<int32_t>(fameResetTime);
		packet.add<int32_t>(mapUnloadTime);
		packet.addString(eventMsg);
		packet.addString(scrollingHeader);
		packet.addString(name);
		packet.addClass<Rates>(rates);
		packet.addClass<MajorBoss>(pianus);
		packet.addClass<MajorBoss>(pap);
		packet.addClass<MajorBoss>(zakum);
		packet.addClass<MajorBoss>(horntail);
		packet.addClass<MajorBoss>(pinkbean);
	}

	auto read(PacketReader &packet) -> void override {
		ribbon = packet.get<int8_t>();
		maxMultiLevel = packet.get<uint8_t>();
		defaultStorageSlots = packet.get<uint8_t>();
		maxStats = packet.get<int16_t>();
		defaultChars = packet.get<int32_t>();
		maxChars = packet.get<int32_t>();
		maxPlayerLoad = packet.get<int32_t>();
		maxChannels = packet.get<int32_t>();
		fameTime = packet.get<int32_t>();
		fameResetTime = packet.get<int32_t>();
		mapUnloadTime = packet.get<int32_t>();
		eventMsg = packet.getString();
		scrollingHeader = packet.getString();
		name = packet.getString();
		rates = packet.getClass<Rates>();
		pianus = packet.getClass<MajorBoss>();
		pap = packet.getClass<MajorBoss>();
		zakum = packet.getClass<MajorBoss>();
		horntail = packet.getClass<MajorBoss>();
		pinkbean = packet.getClass<MajorBoss>();
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
	size_t maxChannels = 20;
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

struct LoginConfig : public IConfigReadable {
	auto read(ConfigFile &conf, const string_t &prefix) -> void override {
		clientEncryption = conf.get<bool>("use_client_encryption");
		clientPing = conf.get<bool>("use_client_ping");
		serverPing = conf.get<bool>("use_inter_ping");
	}

	bool clientEncryption = true;
	bool clientPing = true;
	bool serverPing = true;
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
		loginIp = Ip(Ip::stringToIpv4(conf.get<string_t>("login_ip")));
		port = conf.get<port_t>("login_inter_port");
	}

	port_t port = 0;
	Ip loginIp;
};