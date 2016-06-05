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

#include "common/consume_info.hpp"
#include "common/item_info.hpp"
#include "common/item_reward_info.hpp"
#include "common/pet_info.hpp"
#include "common/pet_interact_info.hpp"
#include "common/scroll_info.hpp"
#include "common/skillbook_info.hpp"
#include "common/summon_bag_info.hpp"
#include "common/types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class buff_data_provider;
	class equip_data_provider;
	class item;

	class item_data_provider {
	public:
		auto load_data(buff_data_provider &provider) -> void;

		auto get_card_id(game_mob_id mob_id) const -> optional<game_item_id>;
		auto get_mob_id(game_item_id card_id) const -> optional<game_mob_id>;
		auto scroll_item(const equip_data_provider &provider, game_item_id scrollId, item *equip, bool white_scroll, bool gm_scroller, int8_t &succeed, bool &cursed) const -> hacking_result;
		auto get_item_info(game_item_id item_id) const -> const item_info * const;
		auto get_consume_info(game_item_id item_id) const -> const consume_info * const;
		auto get_pet_info(game_item_id item_id) const -> const pet_info * const;
		auto get_interaction(game_item_id item_id, int32_t action) const -> const pet_interact_info * const;
		auto get_item_skills(game_item_id item_id) const -> const vector<skillbook_info> * const;
		auto get_item_rewards(game_item_id item_id) const -> const vector<item_reward_info> * const;
		auto get_item_summons(game_item_id item_id) const -> const vector<summon_bag_info> * const;
	private:
		auto load_items() -> void;
		auto load_scrolls() -> void;
		auto load_consumes(buff_data_provider &provider) -> void;
		auto load_map_ranges() -> void;
		auto load_monster_card_data() -> void;
		auto load_item_skills() -> void;
		auto load_summon_bags() -> void;
		auto load_item_rewards() -> void;
		auto load_pets() -> void;
		auto load_pet_interactions() -> void;

		hash_map<game_item_id, item_info> m_item_info;
		hash_map<game_item_id, scroll_info> m_scroll_info;
		hash_map<game_item_id, consume_info> m_consume_info;
		hash_map<game_item_id, vector<summon_bag_info>> m_summon_bags;
		hash_map<game_item_id, vector<skillbook_info>> m_skillbooks;
		hash_map<game_item_id, vector<item_reward_info>> m_item_rewards;
		hash_map<game_item_id, pet_info> m_pet_info;
		hash_map<game_item_id, hash_map<int32_t, pet_interact_info>> m_pet_interact_info;
		hash_map<game_item_id, game_mob_id> m_cards_to_mobs;
		hash_map<game_mob_id, game_item_id> m_mobs_to_cards;
	};
}