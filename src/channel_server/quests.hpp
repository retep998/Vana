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

namespace vana {
	class packet_reader;

	namespace channel_server {
		class player;

		namespace quests {
			auto get_quest(ref_ptr<player> player, packet_reader &reader) -> void;
			auto give_fame(ref_ptr<player> player, game_fame amount) -> result;
			auto give_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty amount, stat_variance variance_policy = stat_variance::none) -> result;
			auto give_mesos(ref_ptr<player> player, game_mesos amount) -> result;
		}
	}
}