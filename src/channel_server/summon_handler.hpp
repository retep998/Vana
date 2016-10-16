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
#include <string>

namespace vana {
	class packet_reader;
	namespace data {
		namespace type {
			class buff_info;
			struct skill_level_info;
		}
	}
	namespace util {
		template <typename TIdentifier> class id_pool;
	}

	namespace channel_server {
		class player;
		struct buff_packet_values;

		namespace summon_messages {
			enum messages : int8_t {
				out_of_time = 0x00,
				disappearing = 0x03,
				none = 0x04
			};
		}

		namespace summon_handler {
			extern vana::util::id_pool<game_summon_id> g_summon_ids;
			auto use_summon(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level) -> void;
			auto remove_summon(ref_ptr<player> player, game_summon_id summon_id, bool packet_only, int8_t show_message, bool from_timer = false) -> void;
			auto show_summon(ref_ptr<player> player) -> void;
			auto show_summons(ref_ptr<player> from_player, ref_ptr<player> to_player) -> void;
			auto move_summon(ref_ptr<player> player, packet_reader &reader) -> void;
			auto damage_summon(ref_ptr<player> player, packet_reader &reader) -> void;
			auto make_buff(ref_ptr<player> player, game_item_id item_id) -> data::type::buff_info;
			auto make_active_buff(ref_ptr<player> player, const data::type::buff_info &data, game_item_id item_id, const data::type::skill_level_info *skill_info) -> buff_packet_values;
			auto summon_skill(ref_ptr<player> player, packet_reader &reader) -> void;
		}
	}
}