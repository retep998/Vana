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

#include "common/IPacket.hpp"
#include "common/Types.hpp"
#include <vector>

namespace vana {
	struct party_data {
		game_party_id id = -1;
		game_player_id leader = -1;
		vector<game_player_id> members;
	};

	template <>
	struct packet_serialize<party_data> {
		auto read(packet_reader &reader) -> party_data {
			party_data ret;
			ret.id = reader.get<game_party_id>();
			ret.leader = reader.get<game_player_id>();
			ret.members = reader.get<vector<game_player_id>>();
			return ret;
		}
		auto write(packet_builder &builder, const party_data &obj) -> void {
			builder.add<game_party_id>(obj.id);
			builder.add<game_player_id>(obj.leader);
			builder.add<vector<game_player_id>>(obj.members);
		}
	};
}