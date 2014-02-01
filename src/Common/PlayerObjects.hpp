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

#include "Ip.hpp"
#include "IPacket.hpp"
#include "Types.hpp"
#include <string>

struct PlayerData : IPacketSerializable {
	PlayerData() :
		ip(0)
	{
	}

	auto copyFrom(const PlayerData &rhs) -> void {
		admin = rhs.admin;
		cashShop = rhs.cashShop;
		channel = rhs.channel;
		map = rhs.map;
		level = rhs.level;
		job = rhs.job;
		gmLevel = rhs.gmLevel;
		ip = rhs.ip;
	}

	auto write(PacketBuilder &builder) const -> void override {
		builder.add<bool>(cashShop);
		builder.add<bool>(admin);
		builder.add<int16_t>(level);
		builder.add<int16_t>(job);
		builder.add<channel_id_t>(channel);
		builder.add<int32_t>(map);
		builder.add<int32_t>(party);
		builder.add<int32_t>(gmLevel);
		builder.add<int32_t>(id);
		builder.addString(name);
		builder.addClass<Ip>(ip);
	}

	auto read(PacketReader &reader) -> void override {
		cashShop = reader.get<bool>();
		admin = reader.get<bool>();
		level = reader.get<int16_t>();
		job = reader.get<int16_t>();
		channel = reader.get<channel_id_t>();
		map = reader.get<int32_t>();
		party = reader.get<int32_t>();
		gmLevel = reader.get<int32_t>();
		id = reader.get<int32_t>();
		name = reader.getString();
		ip = reader.getClass<Ip>();
	}

	bool cashShop = false;
	bool admin = false;
	bool initialized = false;
	int16_t level = -1;
	int16_t job = -1;
	channel_id_t channel = -1;
	int32_t map = -1;
	int32_t party = 0;
	int32_t gmLevel = 0;
	int32_t id = -1;
	string_t name;
	Ip ip;
};

struct PartyData : IPacketSerializable {
	int32_t id = -1;
	int32_t leader = -1;
	vector_t<int32_t> members;

	auto write(PacketBuilder &builder) const -> void override {
		builder.add<int32_t>(id);
		builder.add<int32_t>(leader);
		builder.addVector<int32_t>(members);
	}

	auto read(PacketReader &reader) -> void override {
		id = reader.get<int32_t>();
		leader = reader.get<int32_t>();
		members = reader.getVector<int32_t>();
	}
};