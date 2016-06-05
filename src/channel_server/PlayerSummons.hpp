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

#include "common/types.hpp"
#include "channel_server/Summon.hpp"
#include <vector>

namespace vana {
	class packet_builder;
	class packet_reader;

	namespace channel_server {
		class player;

		class player_summons {
			NONCOPYABLE(player_summons);
			NO_DEFAULT_CONSTRUCTOR(player_summons);
		public:
			player_summons(player *player);

			auto get_summon(game_summon_id summon_id) -> summon *;
			auto add_summon(summon *summon, seconds time) -> void;
			auto remove_summon(game_summon_id summon_id, bool from_timer) -> void;
			auto changed_map() -> void;
			auto for_each(function<void(summon *)> func) -> void;

			auto get_transfer_packet() const -> packet_builder;
			auto parse_transfer_packet(packet_reader &reader) -> void;
		private:
			auto get_summon_time_remaining(game_summon_id summon_id) const -> seconds;

			player *m_player = nullptr;
			vector<summon *> m_summons;
		};
	}
}