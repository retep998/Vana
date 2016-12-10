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
#include "item.hpp"
#include "common/constant/item/max_stat.hpp"
#include "common/data/provider/equip.hpp"
#include "common/soci_extensions.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/misc.hpp"
#include <soci.h>

namespace vana {

const string item::inventory = "inventory";
const string item::storage = "storage";

// The only places that use this constructor set everything manually
item::item(const soci::row &row)
{
	initialize_item(row);
}

item::item(game_item_id item_id, game_slot_qty amount) :
	m_id{item_id},
	m_amount{amount}
{
}

item::item(const data::provider::equip &provider, game_item_id equip_id, stat_variance policy, bool is_gm) :
	m_id{equip_id},
	m_amount{1}
{
	provider.set_equip_stats(this, policy, is_gm, true);
}

item::item(item *item)
{
	m_id = item->get_id();
	m_amount = item->get_amount();
	m_hammers = item->get_hammers();
	m_slots = item->get_slots();
	m_scrolls = item->get_scrolls();
	m_str = item->get_str();
	m_dex = item->get_dex();
	m_int = item->get_int();
	m_luk = item->get_luk();
	m_hp = item->get_hp();
	m_mp = item->get_mp();
	m_watk = item->get_watk();
	m_matk = item->get_matk();
	m_wdef = item->get_wdef();
	m_mdef = item->get_mdef();
	m_accuracy = item->get_accuracy();
	m_avoid = item->get_avoid();
	m_hands = item->get_hands();
	m_jump = item->get_jump();
	m_speed = item->get_speed();
	m_pet_id = item->get_pet_id();
	m_name = item->get_name();
	m_flags = item->get_flags();
	m_expiration = item->get_expiration_time();
}

auto item::has_slip_prevention() const -> bool {
	return test_flags(constant::item::flag::spikes);
}

auto item::has_warm_support() const -> bool {
	return test_flags(constant::item::flag::cold_protection);
}

auto item::has_lock() const -> bool {
	return test_flags(constant::item::flag::lock);
}

auto item::has_karma() const -> bool {
	return test_flags(constant::item::flag::karma_scissors);
}

auto item::has_trade_block() const -> bool {
	return test_flags(constant::item::flag::trade_unavailable);
}

auto item::test_flags(int16_t flags) const -> bool {
	return (m_flags & flags) != 0;
}

auto item::set_prevent_slip(bool prevent) -> void {
	modify_flags(prevent, constant::item::flag::spikes);
}

auto item::set_warm_support(bool warm) -> void {
	modify_flags(warm, constant::item::flag::cold_protection);
}

auto item::set_lock(bool lock) -> void {
	modify_flags(lock, constant::item::flag::lock);
}

auto item::set_karma(bool karma) -> void {
	modify_flags(karma, constant::item::flag::karma_scissors);
}

auto item::set_trade_block(bool block) -> void {
	modify_flags(block, constant::item::flag::trade_unavailable);
}

auto item::modify_flags(bool add, int16_t flags) -> void {
	if (add) {
		m_flags |= flags;
	}
	else {
		m_flags &= ~(flags);
	}
}

auto item::set_str(game_stat strength) -> void {
	m_str = test_stat(strength, constant::item::max_stat::str);
}

auto item::set_dex(game_stat dexterity) -> void {
	m_dex = test_stat(dexterity, constant::item::max_stat::dex);
}

auto item::set_int(game_stat intelligence) -> void {
	m_int = test_stat(intelligence, constant::item::max_stat::intl);
}

auto item::set_luk(game_stat luck) -> void {
	m_luk = test_stat(luck, constant::item::max_stat::luk);
}

auto item::set_hp(game_health hp) -> void {
	m_hp = test_stat(hp, constant::item::max_stat::hp);
}

auto item::set_mp(game_health mp) -> void {
	m_mp = test_stat(mp, constant::item::max_stat::mp);
}

auto item::set_watk(game_stat wAtk) -> void {
	m_watk = test_stat(wAtk, constant::item::max_stat::watk);
}

auto item::set_wdef(game_stat wDef) -> void {
	m_wdef = test_stat(wDef, constant::item::max_stat::wdef);
}

auto item::set_matk(game_stat mAtk) -> void {
	m_matk = test_stat(mAtk, constant::item::max_stat::matk);
}

auto item::set_mdef(game_stat mDef) -> void {
	m_mdef = test_stat(mDef, constant::item::max_stat::mdef);
}

auto item::set_accuracy(game_stat acc) -> void {
	m_accuracy = test_stat(acc, constant::item::max_stat::acc);
}

auto item::set_avoid(game_stat avoid) -> void {
	m_avoid = test_stat(avoid, constant::item::max_stat::avoid);
}

auto item::set_hands(game_stat hands) -> void {
	m_hands = test_stat(hands, constant::item::max_stat::hands);
}

auto item::set_jump(game_stat jump) -> void {
	m_jump = test_stat(jump, constant::item::max_stat::jump);
}

auto item::set_speed(game_stat speed) -> void {
	m_speed = test_stat(speed, constant::item::max_stat::speed);
}

auto item::test_stat(int16_t stat, int16_t max) -> int16_t {
	return stat < 0 ? 0 : (stat > max ? max : stat);
}

auto item::add_str(game_stat strength) -> void {
	set_str(get_str() + strength);
}

auto item::add_dex(game_stat dexterity) -> void {
	set_dex(get_dex() + dexterity);
}

auto item::add_int(game_stat intelligence) -> void {
	set_int(get_int() + intelligence);
}

auto item::add_luk(game_stat luck) -> void {
	set_luk(get_luk() + luck);
}

auto item::add_hp(game_health hp) -> void {
	set_hp(get_hp() + hp);
}

auto item::add_mp(game_health mp) -> void {
	set_mp(get_mp() + mp);
}

auto item::add_watk(game_stat watk) -> void {
	set_watk(get_watk() + watk);
}

auto item::add_wdef(game_stat wdef) -> void {
	set_wdef(get_wdef() + wdef);
}

auto item::add_matk(game_stat matk) -> void {
	set_matk(get_matk() + matk);
}

auto item::add_mdef(game_stat mdef) -> void {
	set_mdef(get_mdef() + mdef);
}

auto item::add_accuracy(game_stat acc) -> void {
	set_accuracy(get_accuracy() + acc);
}

auto item::add_avoid(game_stat avoid) -> void {
	set_avoid(get_avoid() + avoid);
}

auto item::add_hands(game_stat hands) -> void {
	set_hands(get_hands() + hands);
}

auto item::add_jump(game_stat jump) -> void {
	set_jump(get_jump() + jump);
}

auto item::add_speed(game_stat speed) -> void {
	set_speed(get_speed() + speed);
}

auto item::set_amount(game_slot_qty amount) -> void {
	m_amount = amount;
}

auto item::set_slots(int8_t slots) -> void {
	m_slots = slots;
}

auto item::set_pet_id(game_pet_id pet_id) -> void {
	m_pet_id = pet_id;
}

auto item::set_name(const string &name) -> void {
	m_name = name;
}

auto item::initialize_item(const soci::row &row) -> void {
	m_id = row.get<game_item_id>("item_id");
	m_amount = row.get<game_slot_qty>("amount");

	using opt_game_stat = optional<game_stat>;
	using opt_game_health = optional<game_health>;

	opt_int8_t slots = row.get<opt_int8_t>("slots");
	opt_int8_t scrolls = row.get<opt_int8_t>("scrolls");
	opt_game_stat str = row.get<opt_game_stat>("istr");
	opt_game_stat dex = row.get<opt_game_stat>("idex");
	opt_game_stat intt = row.get<opt_game_stat>("iint");
	opt_game_stat luk = row.get<opt_game_stat>("iluk");
	opt_game_health hp = row.get<opt_game_health>("ihp");
	opt_game_health mp = row.get<opt_game_health>("imp");
	opt_game_stat watk = row.get<opt_game_stat>("iwatk");
	opt_game_stat matk = row.get<opt_game_stat>("imatk");
	opt_game_stat wdef = row.get<opt_game_stat>("iwdef");
	opt_game_stat mdef = row.get<opt_game_stat>("imdef");
	opt_game_stat accuracy = row.get<opt_game_stat>("iacc");
	opt_game_stat avoid = row.get<opt_game_stat>("iavo");
	opt_game_stat hands = row.get<opt_game_stat>("ihand");
	opt_game_stat speed = row.get<opt_game_stat>("ispeed");
	opt_game_stat jump = row.get<opt_game_stat>("ijump");
	opt_int16_t flags = row.get<opt_int16_t>("flags");
	opt_int32_t hammers = row.get<opt_int32_t>("hammers");
	optional<game_pet_id> pet_id = row.get<optional<game_pet_id>>("pet_id");
	opt_int64_t expiration = row.get<opt_int64_t>("expiration");
	opt_string name = row.get<opt_string>("name");

	m_slots = slots.get(0);
	m_scrolls = scrolls.get(0);
	m_str = str.get(0);
	m_dex = dex.get(0);
	m_int = intt.get(0);
	m_luk = luk.get(0);
	m_hp = hp.get(0);
	m_mp = mp.get(0);
	m_watk = watk.get(0);
	m_matk = matk.get(0);
	m_wdef = wdef.get(0);
	m_mdef = mdef.get(0);
	m_accuracy = accuracy.get(0);
	m_avoid = avoid.get(0);
	m_hands = hands.get(0);
	m_speed = speed.get(0);
	m_jump = jump.get(0);
	m_hammers = hammers.get(0);
	m_flags = flags.get(0);
	m_expiration = expiration.is_initialized() ?
		file_time{expiration.get()} :
		constant::item::no_expiration;
	m_pet_id = pet_id.get(0);
	m_name = name.get("");
}

auto item::database_insert(vana::io::database &db, const item_db_info &info) -> void {
	vector<item_db_record> v;
	item_db_record r{info, this};
	v.push_back(r);
	item::database_insert(db, v);
}

auto item::database_insert(vana::io::database &db, const vector<item_db_record> &items) -> void {
	using namespace soci;
	auto &sql = db.get_session();
	using vana::util::misc::get_optional;
	using vana::util::nullable_mode;

	static init_list<int8_t> nulls_int8 = {0};
	static init_list<int16_t> nulls_int16 = {0};
	static init_list<int32_t> nulls_int32 = {0};
	static init_list<int64_t> nulls_int64 = {0};
	static init_list<int64_t> nulls_expiration = {0, constant::item::no_expiration.get_value()};
	static init_list<string> nulls_string = {""};

	using opt_game_stat = optional<game_stat>;
	using opt_game_health = optional<game_health>;

	uint8_t inventory = 0;
	game_slot_qty amount = 0;
	game_item_id item_id = 0;
	game_inventory_slot slot = 0;
	game_world_id world_id = 0;
	game_account_id account_id = 0;
	game_player_id player_id = 0;
	string location = "";

	opt_int8_t slots;
	opt_int8_t scrolls;
	opt_game_stat str;
	opt_game_stat dex;
	opt_game_stat intl;
	opt_game_stat luk;
	opt_game_health hp;
	opt_game_health mp;
	opt_game_stat watk;
	opt_game_stat matk;
	opt_game_stat wdef;
	opt_game_stat mdef;
	opt_game_stat acc;
	opt_game_stat avo;
	opt_game_stat hands;
	opt_game_stat speed;
	opt_game_stat jump;
	opt_int16_t flags;
	opt_int32_t hammers;
	optional<game_pet_id> pet_id;
	opt_int64_t expiration;
	opt_string name;

	statement st = (sql.prepare
		<< "INSERT INTO " << db.make_table(vana::table::items) << " (character_id, inv, slot, location, account_id, world_id, item_id, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, pet_id, name, expiration) "
		<< "VALUES (:char, :inv, :slot, :location, :account, :world, :item_id, :amount, :slots, :scrolls, :str, :dex, :int, :luk, :hp, :mp, :watk, :matk, :wdef, :mdef, :acc, :avo, :hands, :speed, :jump, :flags, :hammers, :pet, :name, :expiration)",
		use(player_id, "char"),
		use(inventory, "inv"),
		use(slot, "slot"),
		use(location, "location"),
		use(account_id, "account"),
		use(world_id, "world"),
		use(item_id, "item_id"),
		use(amount, "amount"),
		use(slots, "slots"),
		use(scrolls, "scrolls"),
		use(str, "str"),
		use(dex, "dex"),
		use(intl, "int"),
		use(luk, "luk"),
		use(hp, "hp"),
		use(mp, "mp"),
		use(watk, "watk"),
		use(matk, "matk"),
		use(wdef, "wdef"),
		use(mdef, "mdef"),
		use(acc, "acc"),
		use(avo, "avo"),
		use(hands, "hands"),
		use(speed, "speed"),
		use(jump, "jump"),
		use(flags, "flags"),
		use(hammers, "hammers"),
		use(pet_id, "pet"),
		use(name, "name"),
		use(expiration, "expiration"));

	for (const auto &rec : items) {
		item *item = rec.item;

		location = rec.location;
		account_id = rec.user_id;
		player_id = rec.char_id;
		world_id = rec.world_id;
		slot = rec.slot;
		amount = item->m_amount;
		item_id = item->m_id;
		inventory = vana::util::game_logic::inventory::get_inventory(item_id);
		bool equip = (inventory == constant::inventory::equip);
		nullable_mode nulls = (equip ?
			nullable_mode::null_if_found :
			nullable_mode::force_null);

		nullable_mode equip_only = equip ?
			nullable_mode::null_if_found :
			nullable_mode::force_null;

		nullable_mode equip_only_required = equip ?
			nullable_mode::force_not_null :
			nullable_mode::force_null;

		nullable_mode nonequip_only = equip ?
			nullable_mode::force_null :
			nullable_mode::null_if_found;

		nullable_mode nonequip_only_required = equip ?
			nullable_mode::force_null :
			nullable_mode::force_not_null;

		// Equip only
		slots = get_optional(item->m_slots, equip_only_required, nulls_int8);
		scrolls = get_optional(item->m_scrolls, equip_only_required, nulls_int8);
		str = get_optional(item->m_str, equip_only, nulls_int16);
		dex = get_optional(item->m_dex, equip_only, nulls_int16);
		intl = get_optional(item->m_int, equip_only, nulls_int16);
		luk = get_optional(item->m_luk, equip_only, nulls_int16);
		hp = get_optional(item->m_hp, equip_only, nulls_int16);
		mp = get_optional(item->m_mp, equip_only, nulls_int16);
		watk = get_optional(item->m_watk, equip_only, nulls_int16);
		matk = get_optional(item->m_matk, equip_only, nulls_int16);
		wdef = get_optional(item->m_wdef, equip_only, nulls_int16);
		mdef = get_optional(item->m_mdef, equip_only, nulls_int16);
		acc = get_optional(item->m_accuracy, equip_only, nulls_int16);
		avo = get_optional(item->m_avoid, equip_only, nulls_int16);
		hands = get_optional(item->m_hands, equip_only, nulls_int16);
		speed = get_optional(item->m_speed, equip_only, nulls_int16);
		jump = get_optional(item->m_jump, equip_only, nulls_int16);
		flags = get_optional(item->m_flags, equip_only, nulls_int16);
		hammers = get_optional(item->m_hammers, equip_only, nulls_int32);

		// Non-equip only
		pet_id = get_optional(item->m_pet_id, nonequip_only, nulls_int64);

		// All items
		name = get_optional(item->m_name, nullable_mode::null_if_found, nulls_string);
		expiration = get_optional(item->m_expiration.get_value(), nullable_mode::null_if_found, nulls_expiration);

		st.execute(true);
	}
}

}