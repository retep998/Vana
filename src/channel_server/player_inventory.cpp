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
#include "player_inventory.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/equip.hpp"
#include "common/data/provider/item.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/misc.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/inventory_packet_helper.hpp"
#include "channel_server/map.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/pets_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_packet_helper.hpp"

namespace vana {
namespace channel_server {

player_inventory::player_inventory(ref_ptr<player> player, const array<game_inventory_slot_count, constant::inventory::count> &max_slots, game_mesos mesos) :
	m_max_slots{max_slots},
	m_mesos{mesos},
	m_player{player}
{
	array<game_item_id, 2> init = {0};

	for (size_t i = 0; i < constant::inventory::equipped_slots; ++i) {
		m_equipped[i] = init;
	}

	load();
}

player_inventory::~player_inventory() {
	/* TODO FIXME just convert the damn Item * to ref_ptr_t or owned_ptr_t */
	for (const auto &inv : m_items) {
		std::for_each(std::begin(inv), std::end(inv), [](pair<const game_inventory_slot, item *> p) { delete p.second; });
	}
}

auto player_inventory::load() -> void {
	if (auto player = m_player.lock()) {
		using namespace soci;
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		game_player_id char_id = player->get_id();
		string location = "inventory";

		soci::rowset<> rs = (sql.prepare
			<< "SELECT i.*, p.index, p.name AS pet_name, p.level, p.closeness, p.fullness "
			<< "FROM " << db.make_table(vana::table::items) << " i "
			<< "LEFT OUTER JOIN " << db.make_table(vana::table::pets) << " p ON i.pet_id = p.pet_id "
			<< "WHERE i.location = :location AND i.character_id = :char",
			soci::use(player->get_id(), "char"),
			soci::use(location, "location"));

		for (const auto &row : rs) {
			item *item_record = new item(row);
			add_item(row.get<game_inventory>("inv"), row.get<game_inventory_slot>("slot"), item_record, true);

			if (item_record->get_pet_id() != 0) {
				pet *pet_value = new pet{player.get(), item_record, row};
				player->get_pets()->add_pet(pet_value);
			}
		}

		rs = (sql.prepare << "SELECT t.map_index, t.map_id FROM " << db.make_table(vana::table::teleport_rock_locations) << " t WHERE t.character_id = :char",
			soci::use(player->get_id(), "char"));

		for (const auto &row : rs) {
			int8_t index = row.get<int8_t>("map_index");
			game_map_id map_id = row.get<game_map_id>("map_id");

			if (index >= constant::inventory::teleport_rock_max) {
				m_vip_locations.push_back(map_id);
			}
			else {
				m_rock_locations.push_back(map_id);
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::save() -> void {
	if (auto player = m_player.lock()) {
		using namespace soci;
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		game_player_id char_id = player->get_id();

		sql.once << "DELETE FROM " << db.make_table(vana::table::teleport_rock_locations) << " WHERE character_id = :char",
			use(char_id, "char");

		if (m_rock_locations.size() > 0 || m_vip_locations.size() > 0) {
			game_map_id map_id = 0;
			size_t rock_index = 0;

			statement st = (sql.prepare
				<< "INSERT INTO " << db.make_table(vana::table::teleport_rock_locations) << " "
				<< "VALUES (:char, :i, :map)",
				use(char_id, "char"),
				use(map_id, "map"),
				use(rock_index, "i"));

			for (rock_index = 0; rock_index < m_rock_locations.size(); ++rock_index) {
				map_id = m_rock_locations[rock_index];
				st.execute(true);
			}

			rock_index = constant::inventory::teleport_rock_max;
			for (size_t i = 0; i < m_vip_locations.size(); ++i) {
				map_id = m_vip_locations[i];
				st.execute(true);
				++rock_index;
			}
		}

		sql.once
			<< "DELETE FROM " << db.make_table(vana::table::items) << " "
			<< "WHERE location = :inv AND character_id = :char",
			use(char_id, "char"),
			use(item::inventory, "inv");

		vector<item_db_record> v;
		for (game_inventory i = constant::inventory::equip; i <= constant::inventory::count; ++i) {
			const auto &items_inv = m_items[i - 1];
			for (const auto &kvp : items_inv) {
				item_db_record rec{
					kvp.first,
					char_id,
					player->get_account_id(),
					player->get_world_id(),
					item::inventory,
					kvp.second
				};
				v.push_back(rec);
			}
		}

		item::database_insert(db, v);
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::add_max_slots(game_inventory inventory, game_inventory_slot_count rows) -> void {
	inventory -= 1;

	game_inventory_slot_count &inv = m_max_slots[inventory];
	inv += (rows * 4);

	inv = ext::constrain_range(inv, constant::inventory::min_slots_per_inventory, constant::inventory::max_slots_per_inventory);
	if (auto player = m_player.lock()) {
		player->send(packets::inventory::update_slots(inventory + 1, inv));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::set_mesos(game_mesos mesos, bool send_packet) -> void {
	m_mesos.set_mesos(mesos);
	if (auto player = m_player.lock()) {
		player->send(packets::player::update_stat(constant::stat::mesos, m_mesos.get_mesos(), send_packet));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::modify_mesos(game_mesos mod, bool allow_partial, bool send_packet) -> vana::util::meso_modify_result {
	return modify_mesos_internal(m_mesos.modify_mesos(mod, allow_partial), send_packet);
}

auto player_inventory::add_mesos(game_mesos mod, bool allow_partial, bool send_packet) -> vana::util::meso_modify_result {
	return modify_mesos_internal(m_mesos.add_mesos(mod, allow_partial), send_packet);
}

auto player_inventory::take_mesos(game_mesos mod, bool allow_partial, bool send_packet) -> vana::util::meso_modify_result {
	return modify_mesos_internal(m_mesos.take_mesos(mod, allow_partial), send_packet);
}

auto player_inventory::modify_mesos_internal(vana::util::meso_modify_result query, bool send_packet) -> vana::util::meso_modify_result {
	if (query.get_result() == stack_result::none) {
		return query;
	}

	if (auto player = m_player.lock()) {
		player->send(packets::player::update_stat(constant::stat::mesos, query.get_final_amount(), send_packet));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");

	return query;
}

auto player_inventory::add_item(game_inventory inv, game_inventory_slot slot, item *item, bool is_loading) -> void {
	m_items[inv - 1][slot] = item;
	game_item_id item_id = item->get_id();
	if (m_item_amounts.find(item_id) != std::end(m_item_amounts)) {
		m_item_amounts[item_id] += item->get_amount();
	}
	else {
		m_item_amounts[item_id] = item->get_amount();
	}
	if (slot < 0) {
		add_equipped(slot, item_id);
		if (auto player = m_player.lock()) {
			player->get_stats()->set_equip(slot, item, is_loading);
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
	}
}

auto player_inventory::get_item(game_inventory inv, game_inventory_slot slot) -> item * {
	if (!vana::util::game_logic::inventory::is_valid_inventory(inv)) {
		return nullptr;
	}
	inv -= 1;
	if (m_items[inv].find(slot) != std::end(m_items[inv])) {
		return m_items[inv][slot];
	}
	return nullptr;
}

auto player_inventory::delete_item(game_inventory inv, game_inventory_slot slot, bool update_amount) -> void {
	inv -= 1;
	if (m_items[inv].find(slot) != std::end(m_items[inv])) {
		if (update_amount) {
			item *x = m_items[inv][slot];
			m_item_amounts[x->get_id()] -= x->get_amount();
		}
		if (slot < 0) {
			add_equipped(slot, 0);
			if (auto player = m_player.lock()) {
				player->get_stats()->set_equip(slot, nullptr);
			}
			else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
		}
		delete m_items[inv][slot];
		m_items[inv].erase(slot);
	}
}

auto player_inventory::set_item(game_inventory inv, game_inventory_slot slot, item *item) -> void {
	inv -= 1;
	if (auto player = m_player.lock()) {
		if (item == nullptr) {
			m_items[inv].erase(slot);
			if (slot < 0) {
				add_equipped(slot, 0);
				player->get_stats()->set_equip(slot, nullptr);
				player->get_map()->check_player_equip(player);
			}
		}
		else {
			m_items[inv][slot] = item;
			if (slot < 0) {
				add_equipped(slot, item->get_id());
				player->get_stats()->set_equip(slot, item);
				player->get_map()->check_player_equip(player);
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::destroy_equipped_item(game_item_id item_id) -> void {
	game_inventory inv = constant::inventory::equip;
	const auto &equips = m_items[inv - 1];
	if (auto player = m_player.lock()) {
		for (const auto &kvp : equips) {
			if (kvp.first < 0 && kvp.second->get_id() == item_id) {
				vector<inventory_packet_operation> ops;
				ops.emplace_back(packets::inventory::operation_types::modify_slot, kvp.second, kvp.first);
				player->send(packets::inventory::inventory_operation(true, ops));

				delete_item(inv, kvp.first, false);
				break;
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::get_max_slots(game_inventory inv) const -> game_inventory_slot_count {
	return m_max_slots[inv - 1];
}

auto player_inventory::get_mesos() const -> game_mesos {
	return m_mesos.get_mesos();
}

auto player_inventory::has_any_mesos() const -> bool {
	return m_mesos.has_any();
}

auto player_inventory::can_accept(const vana::util::meso_inventory &other) const -> stack_result {
	return m_mesos.can_accept(other);
}

auto player_inventory::can_modify_mesos(game_mesos mesos) const -> stack_result {
	return m_mesos.can_modify_mesos(mesos);
}

auto player_inventory::can_add_mesos(game_mesos mesos) const -> stack_result {
	return m_mesos.can_add_mesos(mesos);
}

auto player_inventory::can_take_mesos(game_mesos mesos) const -> stack_result {
	return m_mesos.can_take_mesos(mesos);
}

auto player_inventory::get_auto_hp_pot() const -> game_item_id {
	return m_auto_hp_pot_id;
}

auto player_inventory::get_auto_mp_pot() const -> game_item_id {
	return m_auto_mp_pot_id;
}

auto player_inventory::get_item_amount_by_slot(game_inventory inv, game_inventory_slot slot) -> game_slot_qty {
	inv -= 1;
	return (m_items[inv].find(slot) != std::end(m_items[inv]) ? m_items[inv][slot]->get_amount() : 0);
}

auto player_inventory::add_equipped(game_inventory_slot slot, game_item_id item_id) -> void {
	if (std::abs(slot) == constant::equip_slot::mount) {
		if (auto player = m_player.lock()) {
			player->get_mounts()->set_current_mount(item_id);
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
	}

	int8_t cash = vana::util::game_logic::inventory::is_cash_slot(slot) ? 1 : 0;
	m_equipped[vana::util::game_logic::inventory::strip_cash_slot(slot)][cash] = item_id;
}

auto player_inventory::get_equipped_id(game_inventory_slot slot, bool cash) -> game_item_id {
	return m_equipped[slot][(cash ? 1 : 0)];
}

auto player_inventory::add_equipped_packet(packet_builder &builder) -> void {
	for (int8_t i = 0; i < constant::inventory::equipped_slots; ++i) {
		// Shown items
		if (m_equipped[i][0] > 0 || m_equipped[i][1] > 0) {
			builder.add<int8_t>(i);
			if (m_equipped[i][1] <= 0 || (i == constant::equip_slot::weapon && m_equipped[i][0] > 0)) {
				// Normal weapons always here
				builder.add<int32_t>(m_equipped[i][0]);
			}
			else {
				builder.add<int32_t>(m_equipped[i][1]);
			}
		}
	}
	builder.add<int8_t>(-1);
	for (int8_t i = 0; i < constant::inventory::equipped_slots; ++i) {
		// Covered items
		if (m_equipped[i][1] > 0 && m_equipped[i][0] > 0 && i != constant::equip_slot::weapon) {
			builder.add<int8_t>(i);
			builder.add<int32_t>(m_equipped[i][0]);
		}
	}
	builder.add<int8_t>(-1);
	builder.add<int32_t>(m_equipped[constant::equip_slot::weapon][1]); // Cash weapon
}

auto player_inventory::get_item_amount(game_item_id item_id) -> game_slot_qty {
	return m_item_amounts.find(item_id) != std::end(m_item_amounts) ? m_item_amounts[item_id] : 0;
}

auto player_inventory::is_equipped_item(game_item_id item_id) -> bool {
	const auto &equips = m_items[constant::inventory::equip - 1];
	bool has = false;
	for (const auto &kvp : equips) {
		if (kvp.first < 0 && kvp.second->get_id() == item_id) {
			has = true;
			break;
		}
	}
	return has;
}

auto player_inventory::has_open_slots_for(game_item_id item_id, game_slot_qty amount, bool can_stack) -> bool {
	game_slot_qty required = 0;
	game_inventory inv = vana::util::game_logic::inventory::get_inventory(item_id);
	if (!vana::util::game_logic::item::is_stackable(item_id)) {
		required = amount; // These aren't stackable
	}
	else {
		auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);
		game_slot_qty max_slot = item_info->max_slot;
		game_slot_qty existing = get_item_amount(item_id) % max_slot;
		// Bug in global:
		// It doesn't matter if you already have a slot with a partial stack or not, non-shops require at least 1 empty slot
		if (can_stack && existing > 0) {
			// If not, calculate how many slots necessary
			existing += amount;
			if (existing > max_slot) {
				// Only have to bother with required slots if it would put us over the limit of a slot
				required = static_cast<game_slot_qty>(existing / max_slot);
				if ((existing % max_slot) > 0) {
					++required;
				}
			}
		}
		else {
			// If it is, treat it as though no items exist at all
			required = static_cast<game_slot_qty>(amount / max_slot);
			if ((amount % max_slot) > 0) {
				++required;
			}
		}
	}
	return get_open_slots_num(inv) >= required;
}

auto player_inventory::get_open_slots_num(game_inventory inv) -> game_inventory_slot_count {
	game_inventory_slot_count open_slots = 0;
	for (game_inventory_slot_count i = 1; i <= get_max_slots(inv); ++i) {
		if (get_item(inv, i) == nullptr) {
			++open_slots;
		}
	}
	return open_slots;
}

auto player_inventory::do_shadow_stars() -> game_item_id {
	if (auto player = m_player.lock()) {
		for (game_inventory_slot s = 1; s <= get_max_slots(constant::inventory::use); ++s) {
			item *item = get_item(constant::inventory::use, s);
			if (item == nullptr) {
				continue;
			}
			if (vana::util::game_logic::item::is_star(item->get_id()) && item->get_amount() >= constant::item::shadow_stars_cost) {
				inventory::take_item_slot(player, constant::inventory::use, s, constant::item::shadow_stars_cost);
				return item->get_id();
			}
		}
		return 0;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::add_rock_map(game_map_id map_id, int8_t type) -> void {
	const int8_t mode = packets::inventory::rock_modes::add;
	if (auto player = m_player.lock()) {
		if (type == packets::inventory::rock_types::regular) {
			if (m_rock_locations.size() < constant::inventory::teleport_rock_max) {
				m_rock_locations.push_back(map_id);
			}
			player->send(packets::inventory::send_rock_update(mode, type, m_rock_locations));
		}
		else if (type == packets::inventory::rock_types::vip) {
			if (m_vip_locations.size() < constant::inventory::vip_rock_max) {
				m_vip_locations.push_back(map_id);
				// TODO FIXME packet
				// Want packet
			}
			player->send(packets::inventory::send_rock_update(mode, type, m_vip_locations));
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::del_rock_map(game_map_id map_id, int8_t type) -> void {
	const int8_t mode = packets::inventory::rock_modes::remove;

	if (auto player = m_player.lock()) {
		if (type == packets::inventory::rock_types::regular) {
			for (size_t k = 0; k < m_rock_locations.size(); ++k) {
				if (m_rock_locations[k] == map_id) {
					m_rock_locations.erase(std::begin(m_rock_locations) + k);
					player->send(packets::inventory::send_rock_update(mode, type, m_rock_locations));
					break;
				}
			}
		}
		else if (type == packets::inventory::rock_types::vip) {
			for (size_t k = 0; k < m_vip_locations.size(); ++k) {
				if (m_vip_locations[k] == map_id) {
					m_vip_locations.erase(std::begin(m_vip_locations) + k);
					player->send(packets::inventory::send_rock_update(mode, type, m_vip_locations));
					break;
				}
			}
		}
		else THROW_CODE_EXCEPTION(not_implemented_exception);
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::swap_items(int8_t inventory, int16_t slot1, int16_t slot2) -> void {
	bool equipped_slot2 = (slot2 < 0);
	if (auto player = m_player.lock()) {
		if (inventory == constant::inventory::equip && equipped_slot2) {
			// Handle these specially
			item *item1 = get_item(inventory, slot1);
			if (item1 == nullptr) {
				// Hacking
				return;
			}

			game_item_id item_id1 = item1->get_id();
			game_inventory_slot stripped_slot1 = vana::util::game_logic::inventory::strip_cash_slot(slot1);
			game_inventory_slot stripped_slot2 = vana::util::game_logic::inventory::strip_cash_slot(slot2);
			if (!channel_server::get_instance().get_equip_data_provider().is_valid_slot(item_id1, stripped_slot2)) {
				// Hacking
				return;
			}

			auto bind_trade_block_on_equip = [this, slot1, equipped_slot2, item1, item_id1](vector<inventory_packet_operation> &ops) -> bool {
				// We don't care about any case other than equipping because we're checking for gear binds which only happen on first equip
				if (slot1 >= 0 && equipped_slot2) {
					auto &equip_info = channel_server::get_instance().get_equip_data_provider().get_equip_info(item_id1);
					if (equip_info.trade_block_on_equip && !item1->has_trade_block()) {
						item1->set_trade_block(true);
						ops.emplace_back(packets::inventory::operation_types::remove_item, item1, slot1);
						ops.emplace_back(packets::inventory::operation_types::add_item, item1, slot1);
						return true;
					}
				}
				return false;
			};

			item *remove = nullptr;
			game_inventory_slot old_slot = 0;
			bool weapon = (stripped_slot2 == constant::equip_slot::weapon);
			bool shield = (stripped_slot2 == constant::equip_slot::shield);
			bool top = (stripped_slot2 == constant::equip_slot::top);
			bool bottom = (stripped_slot2 == constant::equip_slot::bottom);

			if (weapon && vana::util::game_logic::item::is2h_weapon(item_id1) && get_equipped_id(constant::equip_slot::shield) != 0) {
				old_slot = -constant::equip_slot::shield;
			}
			else if (shield && vana::util::game_logic::item::is2h_weapon(get_equipped_id(constant::equip_slot::weapon))) {
				old_slot = -constant::equip_slot::weapon;
			}
			else if (top && vana::util::game_logic::item::is_overall(item_id1) && get_equipped_id(constant::equip_slot::bottom) != 0) {
				old_slot = -constant::equip_slot::bottom;
			}
			else if (bottom && vana::util::game_logic::item::is_overall(get_equipped_id(constant::equip_slot::top))) {
				old_slot = -constant::equip_slot::top;
			}
			if (old_slot != 0) {
				remove = get_item(inventory, old_slot);
				bool only_swap = true;
				if ((get_equipped_id(constant::equip_slot::shield) != 0) && (get_equipped_id(constant::equip_slot::weapon) != 0)) {
					only_swap = false;
				}
				else if ((get_equipped_id(constant::equip_slot::top) != 0) && (get_equipped_id(constant::equip_slot::bottom) != 0)) {
					only_swap = false;
				}
				if (only_swap) {
					int16_t swap_slot = 0;
					if (weapon) {
						swap_slot = -constant::equip_slot::shield;
						player->get_active_buffs()->swap_weapon();
					}
					else if (shield) {
						swap_slot = -constant::equip_slot::weapon;
						player->get_active_buffs()->swap_weapon();
					}
					else if (top) {
						swap_slot = -constant::equip_slot::bottom;
					}
					else if (bottom) {
						swap_slot = -constant::equip_slot::top;
					}

					set_item(inventory, swap_slot, nullptr);
					set_item(inventory, slot1, remove);
					set_item(inventory, slot2, item1);

					vector<inventory_packet_operation> ops;
					bind_trade_block_on_equip(ops);
					ops.emplace_back(packets::inventory::operation_types::modify_slot, item1, slot1, slot2);
					ops.emplace_back(packets::inventory::operation_types::modify_slot, remove, swap_slot, slot1);
					player->send(packets::inventory::inventory_operation(true, ops));
					player->send_map(packets::inventory::update_player(player));
					return;
				}
				else {
					if (get_open_slots_num(inventory) == 0) {
						player->send(packets::inventory::blank_update());
						return;
					}
					game_inventory_slot free_slot = 0;
					for (game_inventory_slot s = 1; s <= get_max_slots(inventory); s++) {
						item *old_item = get_item(inventory, s);
						if (old_item == nullptr) {
							free_slot = s;
							break;
						}
					}

					set_item(inventory, free_slot, remove);
					set_item(inventory, old_slot, nullptr);

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::modify_slot, item1, old_slot, free_slot);
					player->send(packets::inventory::inventory_operation(true, ops));
				}
			}

			// Nothing special happening, just a simple equip swap
			item *item2 = get_item(inventory, slot2);
			set_item(inventory, slot1, item2);
			set_item(inventory, slot2, item1);

			vector<inventory_packet_operation> ops;
			bind_trade_block_on_equip(ops);
			ops.emplace_back(packets::inventory::operation_types::modify_slot, item1, slot1, slot2);
			player->send(packets::inventory::inventory_operation(true, ops));
		}
		else {
			// The only interesting things that can happen here are stack modifications and slot swapping
			item *item1 = get_item(inventory, slot1);
			item *item2 = get_item(inventory, slot2);

			if (item1 == nullptr) {
				// If item2 is nullptr, it's moving item1 into slot2
				// Hacking
				return;
			}

			game_item_id item_id1 = item1->get_id();
			game_item_id item_id2 = item2 == nullptr ? 0 : item2->get_id();
			if (item2 != nullptr && item_id1 == item_id2 && vana::util::game_logic::item::is_stackable(item_id1)) {
				auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id1);
				game_slot_qty max_slot = item_info->max_slot;

				if (item1->get_amount() + item2->get_amount() <= max_slot) {
					item2->inc_amount(item1->get_amount());
					delete_item(inventory, slot1, false);

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::modify_quantity, item2, slot2);
					ops.emplace_back(packets::inventory::operation_types::modify_slot, item1, slot1);
					player->send(packets::inventory::inventory_operation(true, ops));
				}
				else {
					item1->dec_amount(max_slot - item2->get_amount());
					item2->set_amount(max_slot);

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::modify_quantity, item1, slot1);
					ops.emplace_back(packets::inventory::operation_types::modify_quantity, item2, slot2);
					player->send(packets::inventory::inventory_operation(true, ops));
				}
			}
			else {
				// The item is not stackable, not the same item, or a blank slot swap is occurring, either way it's a plain swap
				set_item(inventory, slot1, item2);
				set_item(inventory, slot2, item1);
				if (item1->get_pet_id() > 0) {
					player->get_pets()->get_pet(item1->get_pet_id())->set_inventory_slot(static_cast<int8_t>(slot2));
				}
				if (item2 != nullptr && item2->get_pet_id() > 0) {
					player->get_pets()->get_pet(item2->get_pet_id())->set_inventory_slot(static_cast<int8_t>(slot1));
				}

				vector<inventory_packet_operation> ops;
				ops.emplace_back(packets::inventory::operation_types::modify_slot, item1, slot1, slot2);
				player->send(packets::inventory::inventory_operation(true, ops));
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::ensure_rock_destination(game_map_id map_id) -> bool {
	for (const auto &location : m_rock_locations) {
		if (location == map_id) {
			return true;
		}
	}
	for (const auto &location : m_vip_locations) {
		if (location == map_id) {
			return true;
		}
	}
	return false;
}

auto player_inventory::add_wish_list_item(game_item_id item_id) -> void {
	m_wishlist.push_back(item_id);
}

auto player_inventory::connect_packet(packet_builder &builder) -> void {
	builder.add<int32_t>(m_mesos.get_mesos());

	for (game_inventory i = constant::inventory::equip; i <= constant::inventory::count; ++i) {
		builder.add<game_inventory_slot_count>(get_max_slots(i));
	}

	// Go through equips
	const auto &equips = m_items[constant::inventory::equip - 1];
	for (const auto &kvp : equips) {
		if (kvp.first < 0 && kvp.first > -100) {
			builder.add_buffer(packets::helpers::add_item_info(kvp.first, kvp.second));
		}
	}
	builder.add<int8_t>(0);
	for (const auto &kvp : equips) {
		if (kvp.first < -100) {
			builder.add_buffer(packets::helpers::add_item_info(kvp.first, kvp.second));
		}
	}
	builder.add<int8_t>(0);
	for (const auto &kvp : equips) {
		if (kvp.first > 0) {
			builder.add_buffer(packets::helpers::add_item_info(kvp.first, kvp.second));
		}
	}
	builder.add<int8_t>(0);

	if (auto player = m_player.lock()) {
		// Equips done, do rest of user's items starting with Use
		for (game_inventory i = constant::inventory::use; i <= constant::inventory::count; ++i) {
			for (game_inventory_slot_count s = 1; s <= get_max_slots(i); ++s) {
				item *item = get_item(i, s);
				if (item == nullptr) {
					continue;
				}
				if (item->get_pet_id() == 0) {
					builder.add_buffer(packets::helpers::add_item_info(s, item));
				}
				else {
					pet *pet = player->get_pets()->get_pet(item->get_pet_id());
					builder.add<int8_t>(static_cast<int8_t>(s));
					builder.add_buffer(packets::pets::add_info(pet, item));
				}
			}
			builder.add<int8_t>(0);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_inventory::rock_packet(packet_builder &builder) -> void {
	builder.add_buffer(packets::helpers::fill_rock_packet(m_rock_locations, constant::inventory::teleport_rock_max));
	builder.add_buffer(packets::helpers::fill_rock_packet(m_vip_locations, constant::inventory::vip_rock_max));
}

auto player_inventory::wishlist_info_packet(packet_builder &builder) -> void {
	builder.add<uint8_t>(static_cast<uint8_t>(m_wishlist.size()));
	for (const auto &item : m_wishlist) {
		builder.add<int32_t>(item);
	}
}

auto player_inventory::check_expired_items() -> void {
	vector<game_item_id> expired_item_ids;
	file_time server_time{};

	if (auto player = m_player.lock()) {
		for (game_inventory i = constant::inventory::equip; i <= constant::inventory::count; ++i) {
			for (game_inventory_slot_count s = 1; s <= get_max_slots(i); ++s) {
				if (item *item = get_item(i, s)) {
					if (item->get_expiration_time() != constant::item::no_expiration && item->get_expiration_time() <= server_time) {
						expired_item_ids.push_back(item->get_id());
						inventory::take_item_slot(player, i, s, item->get_amount());
					}
				}
			}
		}

		if (expired_item_ids.size() > 0) {
			if (auto player = m_player.lock()) {
				player->send(packets::inventory::send_item_expired(expired_item_ids));
			}
			else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

}
}