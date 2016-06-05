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

#include "common/ip_temp.hpp"
#include "common/i_packet.hpp"
#include "common/types_temp.hpp"
#include <string>
#include <vector>

namespace vana {
	struct player_data {
		player_data() :
			ip{0}
		{
		}

		auto copy_from(const player_data &rhs) -> void {
			admin = rhs.admin;
			cash_shop = rhs.cash_shop;
			mts = rhs.mts;
			channel = rhs.channel;
			map = rhs.map;
			level = rhs.level;
			job = rhs.job;
			gm_level = rhs.gm_level;
			ip = rhs.ip;
			mutual_buddies = rhs.mutual_buddies;
		}

		bool cash_shop = false;
		bool mts = false;
		bool admin = false;
		bool initialized = false;
		bool transferring = false;
		optional<game_player_level> level;
		optional<game_job_id> job;
		optional<game_channel_id> channel;
		optional<game_map_id> map;
		game_party_id party = 0;
		int32_t gm_level = 0;
		game_player_id id = -1;
		string name;
		ip ip;
		vector<game_player_id> mutual_buddies;
	};

	template <>
	struct packet_serialize<player_data> {
		auto read(packet_reader &reader) -> player_data {
			player_data ret;
			ret.transferring = reader.get<bool>();
			ret.cash_shop = reader.get<bool>();
			ret.mts = reader.get<bool>();
			ret.admin = reader.get<bool>();
			ret.level = reader.get<optional<game_player_level>>();
			ret.job = reader.get<optional<game_job_id>>();
			ret.channel = reader.get<optional<game_channel_id>>();
			ret.map = reader.get<optional<game_map_id>>();
			ret.party = reader.get<game_party_id>();
			ret.gm_level = reader.get<int32_t>();
			ret.id = reader.get<game_player_id>();
			ret.name = reader.get<string>();
			ret.ip = reader.get<ip>();
			ret.mutual_buddies = reader.get<vector<game_player_id>>();
			return ret;
		}
		auto write(packet_builder &builder, const player_data &obj) -> void {
			builder.add<bool>(obj.transferring);
			builder.add<bool>(obj.cash_shop);
			builder.add<bool>(obj.mts);
			builder.add<bool>(obj.admin);
			builder.add<optional<game_player_level>>(obj.level);
			builder.add<optional<game_job_id>>(obj.job);
			builder.add<optional<game_channel_id>>(obj.channel);
			builder.add<optional<game_map_id>>(obj.map);
			builder.add<game_party_id>(obj.party);
			builder.add<int32_t>(obj.gm_level);
			builder.add<game_player_id>(obj.id);
			builder.add<string>(obj.name);
			builder.add<ip>(obj.ip);
			builder.add<vector<game_player_id>>(obj.mutual_buddies);
		}
	};
}