/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/Ip.hpp"
#include "Common/IPacket.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	struct PlayerData {
		PlayerData() :
			ip{0}
		{
		}

		auto copyFrom(const PlayerData &rhs) -> void {
			admin = rhs.admin;
			cashShop = rhs.cashShop;
			mts = rhs.mts;
			channel = rhs.channel;
			map = rhs.map;
			level = rhs.level;
			job = rhs.job;
			gmLevel = rhs.gmLevel;
			ip = rhs.ip;
			mutualBuddies = rhs.mutualBuddies;
		}

		bool cashShop = false;
		bool mts = false;
		bool admin = false;
		bool initialized = false;
		bool transferring = false;
		optional_t<player_level_t> level;
		optional_t<job_id_t> job;
		optional_t<channel_id_t> channel;
		optional_t<map_id_t> map;
		party_id_t party = 0;
		int32_t gmLevel = 0;
		player_id_t id = -1;
		string_t name;
		Ip ip;
		vector_t<player_id_t> mutualBuddies;
	};

	template <>
	struct PacketSerialize<PlayerData> {
		auto read(PacketReader &reader) -> PlayerData {
			PlayerData ret;
			ret.transferring = reader.get<bool>();
			ret.cashShop = reader.get<bool>();
			ret.mts = reader.get<bool>();
			ret.admin = reader.get<bool>();
			ret.level = reader.get<optional_t<player_level_t>>();
			ret.job = reader.get<optional_t<job_id_t>>();
			ret.channel = reader.get<optional_t<channel_id_t>>();
			ret.map = reader.get<optional_t<map_id_t>>();
			ret.party = reader.get<party_id_t>();
			ret.gmLevel = reader.get<int32_t>();
			ret.id = reader.get<player_id_t>();
			ret.name = reader.get<string_t>();
			ret.ip = reader.get<Ip>();
			ret.mutualBuddies = reader.get<vector_t<player_id_t>>();
			return ret;
		}
		auto write(PacketBuilder &builder, const PlayerData &obj) -> void {
			builder.add<bool>(obj.transferring);
			builder.add<bool>(obj.cashShop);
			builder.add<bool>(obj.mts);
			builder.add<bool>(obj.admin);
			builder.add<optional_t<player_level_t>>(obj.level);
			builder.add<optional_t<job_id_t>>(obj.job);
			builder.add<optional_t<channel_id_t>>(obj.channel);
			builder.add<optional_t<map_id_t>>(obj.map);
			builder.add<party_id_t>(obj.party);
			builder.add<int32_t>(obj.gmLevel);
			builder.add<player_id_t>(obj.id);
			builder.add<string_t>(obj.name);
			builder.add<Ip>(obj.ip);
			builder.add<vector_t<player_id_t>>(obj.mutualBuddies);
		}
	};
}