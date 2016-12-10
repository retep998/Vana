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

#include "common/file_time.hpp"
#include "common/constant/item.hpp"
#include "common/item_db_information.hpp"
#include "common/item_db_record.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	class soci::row;

	namespace data {
		namespace provider {
			class equip;
		}
	}
	namespace io {
		class database;
	}

	class item {
	public:
		item() = default;
		item(const soci::row &row);
		item(game_item_id item_id, game_slot_qty amount);
		item(const data::provider::equip &provider, game_item_id equip_id, stat_variance policy, bool is_gm);
		item(item *item);

		auto has_warm_support() const -> bool;
		auto has_slip_prevention() const -> bool;
		auto has_lock() const -> bool;
		auto has_karma() const -> bool;
		auto has_trade_block() const -> bool;

		auto get_slots() const -> int8_t { return m_slots; }
		auto get_scrolls() const -> int8_t { return m_scrolls; }
		auto get_str() const -> game_stat { return m_str; }
		auto get_dex() const -> game_stat { return m_dex; }
		auto get_int() const -> game_stat { return m_int; }
		auto get_luk() const -> game_stat { return m_luk; }
		auto get_hp() const -> game_health { return m_hp; }
		auto get_mp() const -> game_health { return m_mp; }
		auto get_watk() const -> game_stat { return m_watk; }
		auto get_matk() const -> game_stat { return m_matk; }
		auto get_wdef() const -> game_stat { return m_wdef; }
		auto get_mdef() const -> game_stat { return m_mdef; }
		auto get_accuracy() const -> game_stat { return m_accuracy; }
		auto get_avoid() const -> game_stat { return m_avoid; }
		auto get_hands() const -> game_stat { return m_hands; }
		auto get_speed() const -> game_stat { return m_speed; }
		auto get_jump() const -> game_stat { return m_jump; }
		auto get_amount() const -> game_slot_qty { return m_amount; }
		auto get_flags() const -> int16_t { return m_flags; }
		auto get_id() const -> game_item_id { return m_id; }
		auto get_hammers() const -> int32_t { return m_hammers; }
		auto get_pet_id() const -> game_pet_id { return m_pet_id; }
		auto get_expiration_time() const -> file_time { return m_expiration; }
		auto get_name() const -> const string & { return m_name; }

		auto set_prevent_slip(bool prevent) -> void;
		auto set_warm_support(bool warm) -> void;
		auto set_lock(bool lock) -> void;
		auto set_karma(bool karma) -> void;
		auto set_trade_block(bool block) -> void;

		auto database_insert(vana::io::database &db, const item_db_info &info) -> void;
		auto set_slots(int8_t slots) -> void;
		auto set_str(game_stat strength) -> void;
		auto set_dex(game_stat dexterity) -> void;
		auto set_int(game_stat intelligence) -> void;
		auto set_luk(game_stat luck) -> void;
		auto set_hp(game_health hp) -> void;
		auto set_mp(game_health mp) -> void;
		auto set_watk(game_stat watk) -> void;
		auto set_wdef(game_stat wdef) -> void;
		auto set_matk(game_stat matk) -> void;
		auto set_mdef(game_stat mdef) -> void;
		auto set_accuracy(game_stat acc) -> void;
		auto set_avoid(game_stat avoid) -> void;
		auto set_hands(game_stat hands) -> void;
		auto set_jump(game_stat jump) -> void;
		auto set_speed(game_stat speed) -> void;
		auto set_amount(game_slot_qty amount) -> void;
		auto set_name(const string &name) -> void;
		auto set_pet_id(game_pet_id pet_id) -> void;
		auto add_str(game_stat strength) -> void;
		auto add_dex(game_stat dexterity) -> void;
		auto add_int(game_stat intelligence) -> void;
		auto add_luk(game_stat luck) -> void;
		auto add_hp(game_health hp) -> void;
		auto add_mp(game_health mp) -> void;
		auto add_watk(game_stat watk) -> void;
		auto add_wdef(game_stat wdef) -> void;
		auto add_matk(game_stat matk) -> void;
		auto add_mdef(game_stat mdef) -> void;
		auto add_accuracy(game_stat acc) -> void;
		auto add_avoid(game_stat avoid) -> void;
		auto add_hands(game_stat hands) -> void;
		auto add_jump(game_stat jump) -> void;
		auto add_speed(game_stat speed) -> void;
		auto inc_amount(game_slot_qty mod) -> void { m_amount += mod; }
		auto dec_amount(game_slot_qty mod) -> void { m_amount -= mod; }
		auto inc_hammers() -> void { m_hammers++; }
		auto inc_slots(int8_t inc = 1) -> void { m_slots += inc; }
		auto dec_slots(int8_t dec = 1) -> void { m_slots -= dec; }
		auto inc_scrolls() -> void { m_scrolls++; }

		static auto database_insert(vana::io::database &db, const vector<item_db_record> &items) -> void;

		const static string inventory;
		const static string storage;
	private:
		auto test_stat(int16_t stat, int16_t max) -> int16_t;
		auto modify_flags(bool add, int16_t flags) -> void;
		auto test_flags(int16_t flags) const -> bool;
		auto initialize_item(const soci::row &row) -> void;

		int8_t m_slots = 0;
		int8_t m_scrolls = 0;
		game_stat m_str = 0;
		game_stat m_dex = 0;
		game_stat m_int = 0;
		game_stat m_luk = 0;
		game_health m_hp = 0;
		game_health m_mp = 0;
		game_stat m_watk = 0;
		game_stat m_matk = 0;
		game_stat m_wdef = 0;
		game_stat m_mdef = 0;
		game_stat m_accuracy = 0;
		game_stat m_avoid = 0;
		game_stat m_hands = 0;
		game_stat m_jump = 0;
		game_stat m_speed = 0;
		int16_t m_flags = 0;
		game_slot_qty m_amount = 0;
		game_item_id m_id = 0;
		int32_t m_hammers = 0;
		game_pet_id m_pet_id = 0;
		file_time m_expiration = constant::item::no_expiration;
		string m_name;
	};
}