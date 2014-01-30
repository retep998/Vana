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

	auto write(PacketCreator &packet) const -> void override {
		packet.add<bool>(cashShop);
		packet.add<bool>(admin);
		packet.add<int16_t>(level);
		packet.add<int16_t>(job);
		packet.add<channel_id_t>(channel);
		packet.add<int32_t>(map);
		packet.add<int32_t>(party);
		packet.add<int32_t>(gmLevel);
		packet.add<int32_t>(id);
		packet.addString(name);
		packet.addClass<Ip>(ip);
	}

	auto read(PacketReader &packet) -> void override {
		cashShop = packet.get<bool>();
		admin = packet.get<bool>();
		level = packet.get<int16_t>();
		job = packet.get<int16_t>();
		channel = packet.get<channel_id_t>();
		map = packet.get<int32_t>();
		party = packet.get<int32_t>();
		gmLevel = packet.get<int32_t>();
		id = packet.get<int32_t>();
		name = packet.getString();
		ip = packet.getClass<Ip>();
	}

	bool cashShop = false;
	bool admin = false;
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

	auto write(PacketCreator &packet) const -> void override {
		packet.add<int32_t>(id);
		packet.add<int32_t>(leader);
		packet.addVector<int32_t>(members);
	}

	auto read(PacketReader &packet) -> void override {
		id = packet.get<int32_t>();
		leader = packet.get<int32_t>();
		members = packet.getVector<int32_t>();
	}
};