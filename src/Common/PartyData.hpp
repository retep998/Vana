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

#include "IPacket.hpp"
#include "Types.hpp"
#include <vector>

namespace Vana {
	struct PartyData {
		party_id_t id = -1;
		player_id_t leader = -1;
		vector_t<player_id_t> members;
	};

	template <>
	struct PacketSerialize<PartyData> {
		auto read(PacketReader &reader) -> PartyData {
			PartyData ret;
			ret.id = reader.get<party_id_t>();
			ret.leader = reader.get<player_id_t>();
			ret.members = reader.get<vector_t<player_id_t>>();
			return ret;
		}
		auto write(PacketBuilder &builder, const PartyData &obj) -> void {
			builder.add<party_id_t>(obj.id);
			builder.add<player_id_t>(obj.leader);
			builder.add<vector_t<player_id_t>>(obj.members);
		}
	};
}