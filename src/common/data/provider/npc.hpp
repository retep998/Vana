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

#include "common/data/type/npc_info.hpp"
#include "common/types.hpp"
#include <unordered_map>

namespace vana {
	namespace data {
		namespace provider {
			class npc {
			public:
				auto load_data() -> void;

				auto get_storage_cost(game_npc_id npc) const -> game_mesos;
				auto is_maple_tv(game_npc_id npc) const -> bool;
				auto is_guild_rank(game_npc_id npc) const -> bool;
				auto is_valid_npc_id(game_npc_id npc) const -> bool;
			private:
				hash_map<game_npc_id, data::type::npc_info> m_data;
			};
		}
	}
}