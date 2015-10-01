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

#include "Common/Types.hpp"
#include "ChannelServer/KeyMapAction.hpp"
#include "ChannelServer/KeyMapKey.hpp"
#include "ChannelServer/KeyMapType.hpp"
#include <unordered_map>

namespace Vana {
	namespace ChannelServer {
		class KeyMaps {
			NONCOPYABLE(KeyMaps);
		public:
			struct KeyMap;

			KeyMaps() = default;

			auto add(KeyMapKey pos, const KeyMap &map) -> void;
			auto add(int32_t pos, const KeyMap &map) -> void;
			auto defaultMap() -> void;
			auto getKeyMap(int32_t pos) -> KeyMap *;
			auto getMax() -> int32_t;

			auto load(player_id_t charId) -> void;
			auto save(player_id_t charId) -> void;

			static const size_t KeyCount = 90;
		private:
			hash_map_t<int32_t, KeyMap> m_keyMaps;
			int32_t m_maxValue = -1; // Cache max value
		};

		struct KeyMaps::KeyMap {
			KeyMap(KeyMapAction action);
			KeyMap(KeyMapType type, int32_t action);
			KeyMap() = default;

			KeyMapType type = KeyMapType::None;
			int32_t action = 0;
		};

		inline
		auto KeyMaps::add(KeyMapKey pos, const KeyMap &map) -> void {
			add(static_cast<int32_t>(pos), map);
		}

		inline
		auto KeyMaps::add(int32_t pos, const KeyMap &map) -> void {
			if (map.type == KeyMapType::None) {
				auto iter = m_keyMaps.find(pos);
				if (iter != std::end(m_keyMaps)) {
					m_keyMaps.erase(pos);
					if (m_maxValue == pos) {
						m_maxValue = -1;
						for (const auto &kvp : m_keyMaps) {
							if (kvp.first > m_maxValue) {
								m_maxValue = kvp.first;
							}
						}
					}
				}
			}
			else {
				m_keyMaps[pos] = map;
				if (m_maxValue < pos) {
					m_maxValue = pos;
				}
			}
		}

		inline
		auto KeyMaps::getKeyMap(int32_t pos) -> KeyMaps::KeyMap * {
			auto kvp = m_keyMaps.find(pos);
			if (kvp != std::end(m_keyMaps)) {
				return &kvp->second;
			}
			return nullptr;
		}

		inline
		auto KeyMaps::getMax() -> int32_t {
			return m_maxValue;
		}

		inline
		KeyMaps::KeyMap::KeyMap(KeyMapAction action) :
			action{static_cast<int32_t>(action)}
		{
			switch (action) {
				case KeyMapAction::Cockeyed:
				case KeyMapAction::Happy:
				case KeyMapAction::Sarcastic:
				case KeyMapAction::Crying:
				case KeyMapAction::Outraged:
				case KeyMapAction::Shocked:
				case KeyMapAction::Annoyed:
					type = KeyMapType::BasicFace;
					break;
				case KeyMapAction::PickUp:
				case KeyMapAction::Sit:
				case KeyMapAction::Attack:
				case KeyMapAction::Jump:
				case KeyMapAction::NpcChat:
					type = KeyMapType::BasicAction;
					break;
				case KeyMapAction::EquipmentMenu:
				case KeyMapAction::ItemMenu:
				case KeyMapAction::AbilityMenu:
				case KeyMapAction::SkillMenu:
				case KeyMapAction::BuddyList:
				case KeyMapAction::WorldMap:
				case KeyMapAction::Messenger:
				case KeyMapAction::Minimap:
				case KeyMapAction::QuestMenu:
				case KeyMapAction::SetKey:
				case KeyMapAction::AllChat:
				case KeyMapAction::WhisperChat:
				case KeyMapAction::PartyChat:
				case KeyMapAction::BuddyChat:
				case KeyMapAction::Shortcut:
				case KeyMapAction::QuickSlot:
				case KeyMapAction::ExpandChat:
				case KeyMapAction::GuildList:
				case KeyMapAction::GuildChat:
				case KeyMapAction::PartyList:
				case KeyMapAction::Helper:
				case KeyMapAction::SpouseChat:
				case KeyMapAction::MonsterBook:
				case KeyMapAction::CashShop:
				case KeyMapAction::AllianceChat:
				case KeyMapAction::PartySearch:
				case KeyMapAction::FamilyList:
					type = KeyMapType::Menu;
					break;
				default:
					throw NotImplementedException{"KeyMapAction"};
			}
		}

		inline
		KeyMaps::KeyMap::KeyMap(KeyMapType type, int32_t action) :
			type{type},
			action{action}
		{
		}
	}
}