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
#include <unordered_map>

namespace vana {
	class packet_builder;

	namespace channel_server {
		class pet;
		class player;

		class player_pets {
			NONCOPYABLE(player_pets);
			NO_DEFAULT_CONSTRUCTOR(player_pets);
		public:
			player_pets(player *player);

			auto save() -> void;
			auto pet_info_packet(packet_builder &builder) -> void;
			auto connect_packet(packet_builder &builder) -> void;

			auto get_pet(game_pet_id pet_id) -> pet *;
			auto get_summoned(int8_t index) -> pet *;

			auto add_pet(pet *pet) -> void;
			auto set_summoned(int8_t index, game_pet_id pet_id) -> void;
		private:
			player *m_player = nullptr;
			hash_map<game_pet_id, pet *> m_pets;
			hash_map<int8_t, game_pet_id> m_summoned;
		};
	}
}