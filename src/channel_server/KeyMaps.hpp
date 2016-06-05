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

#include "common/Types.hpp"
#include "channel_server/KeyMapAction.hpp"
#include "channel_server/KeyMapKey.hpp"
#include "channel_server/KeyMapType.hpp"
#include <unordered_map>

namespace vana {
	namespace channel_server {
		class key_maps {
			NONCOPYABLE(key_maps);
		public:
			struct key_map;

			key_maps() = default;

			auto add(key_map_key pos, const key_map &map) -> void;
			auto add(int32_t pos, const key_map &map) -> void;
			auto default_map() -> void;
			auto get_key_map(int32_t pos) -> key_map *;
			auto get_max() -> int32_t;

			auto load(game_player_id char_id) -> void;
			auto save(game_player_id char_id) -> void;

			static const size_t key_count = 90;
		private:
			hash_map<int32_t, key_map> m_key_maps;
			int32_t m_max_value = -1; // Cache max value
		};

		struct key_maps::key_map {
			key_map(key_map_action action);
			key_map(key_map_type type, int32_t action);
			key_map() = default;

			key_map_type type = key_map_type::none;
			int32_t action = 0;
		};

		inline
		auto key_maps::add(key_map_key pos, const key_map &map) -> void {
			add(static_cast<int32_t>(pos), map);
		}

		inline
		auto key_maps::add(int32_t pos, const key_map &map) -> void {
			if (map.type == key_map_type::none) {
				auto iter = m_key_maps.find(pos);
				if (iter != std::end(m_key_maps)) {
					m_key_maps.erase(pos);
					if (m_max_value == pos) {
						m_max_value = -1;
						for (const auto &kvp : m_key_maps) {
							if (kvp.first > m_max_value) {
								m_max_value = kvp.first;
							}
						}
					}
				}
			}
			else {
				m_key_maps[pos] = map;
				if (m_max_value < pos) {
					m_max_value = pos;
				}
			}
		}

		inline
		auto key_maps::get_key_map(int32_t pos) -> key_maps::key_map * {
			auto kvp = m_key_maps.find(pos);
			if (kvp != std::end(m_key_maps)) {
				return &kvp->second;
			}
			return nullptr;
		}

		inline
		auto key_maps::get_max() -> int32_t {
			return m_max_value;
		}

		inline
		key_maps::key_map::key_map(key_map_action action) :
			action{static_cast<int32_t>(action)}
		{
			switch (action) {
				case key_map_action::cockeyed:
				case key_map_action::happy:
				case key_map_action::sarcastic:
				case key_map_action::crying:
				case key_map_action::outraged:
				case key_map_action::shocked:
				case key_map_action::annoyed:
					type = key_map_type::basic_face;
					break;
				case key_map_action::pick_up:
				case key_map_action::sit:
				case key_map_action::attack:
				case key_map_action::jump:
				case key_map_action::npc_chat:
					type = key_map_type::basic_action;
					break;
				case key_map_action::equipment_menu:
				case key_map_action::item_menu:
				case key_map_action::ability_menu:
				case key_map_action::skill_menu:
				case key_map_action::buddy_list:
				case key_map_action::world_map:
				case key_map_action::messenger:
				case key_map_action::minimap:
				case key_map_action::quest_menu:
				case key_map_action::set_key:
				case key_map_action::all_chat:
				case key_map_action::whisper_chat:
				case key_map_action::party_chat:
				case key_map_action::buddy_chat:
				case key_map_action::shortcut:
				case key_map_action::quick_slot:
				case key_map_action::expand_chat:
				case key_map_action::guild_list:
				case key_map_action::guild_chat:
				case key_map_action::party_list:
				case key_map_action::helper:
				case key_map_action::spouse_chat:
				case key_map_action::monster_book:
				case key_map_action::cash_shop:
				case key_map_action::alliance_chat:
				case key_map_action::party_search:
				case key_map_action::family_list:
					type = key_map_type::menu;
					break;
				default:
					throw not_implemented_exception{"key_map_action"};
			}
		}

		inline
		key_maps::key_map::key_map(key_map_type type, int32_t action) :
			type{type},
			action{action}
		{
		}
	}
}