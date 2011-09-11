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
#pragma once

#include "ConfigFile.h"
#include "IConfig.h"
#include "IPacket.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct MajorBoss : public IPacketSerializable<MajorBoss> {
	int16_t attempts;
	vector<int8_t> channels;

	void write(PacketCreator &packet) {
		packet.add<int16_t>(attempts);
		packet.addVector(channels);
	}
	void read(PacketReader &packet) {
		attempts = packet.get<int16_t>();
		channels = packet.getVector<int8_t>();
	}
};

struct Configuration : public IPacketSerializable<Configuration> {
	bool clientEncryption;
	int8_t ribbon;
	uint8_t maxMultiLevel;
	int16_t maxStats;
	int32_t expRate;
	int32_t questExpRate;
	int32_t mesoRate;
	int32_t dropRate;
	int32_t maxChars;
	int32_t maxPlayerLoad;
	int32_t fameTime;
	int32_t fameResetTime;
	size_t maxChannels;
	string eventMsg;
	string scrollingHeader;
	string name;
	MajorBoss pianus;
	MajorBoss pap;
	MajorBoss zakum;
	MajorBoss horntail;
	MajorBoss pinkbean;

	void write(PacketCreator &packet) {
		packet.addBool(clientEncryption);
		packet.add<int8_t>(ribbon);
		packet.add<uint8_t>(maxMultiLevel);
		packet.add<int16_t>(maxStats);
		packet.add<int32_t>(expRate);
		packet.add<int32_t>(questExpRate);
		packet.add<int32_t>(mesoRate);
		packet.add<int32_t>(dropRate);
		packet.add<int32_t>(maxChars);
		packet.add<int32_t>(maxPlayerLoad);
		packet.add<int32_t>(maxChannels);
		packet.add<int32_t>(fameTime);
		packet.add<int32_t>(fameResetTime);
		packet.addString(eventMsg);
		packet.addString(scrollingHeader);
		packet.addString(name);
		packet.addClass<MajorBoss>(pianus);
		packet.addClass<MajorBoss>(pap);
		packet.addClass<MajorBoss>(zakum);
		packet.addClass<MajorBoss>(horntail);
		packet.addClass<MajorBoss>(pinkbean);
	}
	void read(PacketReader &packet) {
		clientEncryption = packet.getBool();
		ribbon = packet.get<int8_t>();
		maxMultiLevel = packet.get<uint8_t>();
		maxStats = packet.get<int16_t>();
		expRate = packet.get<int32_t>();
		questExpRate = packet.get<int32_t>();
		mesoRate = packet.get<int32_t>();
		dropRate = packet.get<int32_t>();
		maxChars = packet.get<int32_t>();
		maxPlayerLoad = packet.get<int32_t>();
		maxChannels = packet.get<int32_t>();
		fameTime = packet.get<int32_t>();
		fameResetTime = packet.get<int32_t>();
		eventMsg = packet.getString();
		scrollingHeader = packet.getString();
		name = packet.getString();
		pianus = packet.getClass<MajorBoss>();
		pap = packet.getClass<MajorBoss>();
		zakum = packet.getClass<MajorBoss>();
		horntail = packet.getClass<MajorBoss>();
		pinkbean = packet.getClass<MajorBoss>();
	}
};

struct LogConfig : IConfigReadable<LogConfig> {
	int32_t destination;
	uint32_t bufferSize;
	string format;
	string timeFormat;
	string file;

	void read(ConfigFile &conf, const string &prefix) {
		string &t = prefix + "_log_";
		destination = conf.get<int32_t>(t + "destination");
		bufferSize = conf.get<uint32_t>(t + "buffer_size");
		format = conf.getString(t + "format");
		file = conf.getString(t + "file");
		timeFormat = conf.getString("log_time_format");
	}
};

struct DbConfig : IConfigReadable<DbConfig> {
	string database;
	string host;
	string username;
	string password;
	port_t port;

	void read(ConfigFile &conf, const string &prefix) {
		string &t = prefix + "_";
		database = conf.getString(t + "database");
		host = conf.getString(t + "host");
		username = conf.getString(t + "username");
		password = conf.getString(t + "password");
		port = conf.get<port_t>(t + "port");
	}
};