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

struct PlayerData {
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

template <>
struct PacketSerialize<PlayerData> {
	auto read(PacketReader &reader) -> PlayerData {
		PlayerData ret;
		ret.cashShop = reader.get<bool>();
		ret.admin = reader.get<bool>();
		ret.level = reader.get<int16_t>();
		ret.job = reader.get<int16_t>();
		ret.channel = reader.get<channel_id_t>();
		ret.map = reader.get<int32_t>();
		ret.party = reader.get<int32_t>();
		ret.gmLevel = reader.get<int32_t>();
		ret.id = reader.get<int32_t>();
		ret.name = reader.get<string_t>();
		ret.ip = reader.get<Ip>();
		return ret;
	}
	auto write(PacketBuilder &builder, const PlayerData &obj) -> void {
		builder.add<bool>(obj.cashShop);
		builder.add<bool>(obj.admin);
		builder.add<int16_t>(obj.level);
		builder.add<int16_t>(obj.job);
		builder.add<channel_id_t>(obj.channel);
		builder.add<int32_t>(obj.map);
		builder.add<int32_t>(obj.party);
		builder.add<int32_t>(obj.gmLevel);
		builder.add<int32_t>(obj.id);
		builder.add<string_t>(obj.name);
		builder.add<Ip>(obj.ip);
	}
};

struct PartyData {
	int32_t id = -1;
	int32_t leader = -1;
	vector_t<int32_t> members;
};

template <>
struct PacketSerialize<PartyData> {
	auto read(PacketReader &reader) -> PartyData {
		PartyData ret;
		ret.id = reader.get<int32_t>();
		ret.leader = reader.get<int32_t>();
		ret.members = reader.get<vector_t<int32_t>>();
		return ret;
	}
	auto write(PacketBuilder &builder, const PartyData &obj) -> void {
		builder.add<int32_t>(obj.id);
		builder.add<int32_t>(obj.leader);
		builder.add<vector_t<int32_t>>(obj.members);
	}
};