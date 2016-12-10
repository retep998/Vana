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
#include "player_pets.hpp"
#include "common/io/database.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

player_pets::player_pets(ref_ptr<player> player) :
	m_player{player}
{
}

auto player_pets::add_pet(pet *pet) -> void {
	m_pets[pet->get_id()] = pet;

	if (pet->is_summoned()) {
		set_summoned(pet->get_index().get(), pet->get_id());
	}
}

auto player_pets::get_pet(game_pet_id pet_id) -> pet * {
	return m_pets.find(pet_id) != std::end(m_pets) ? m_pets[pet_id] : nullptr;
}

auto player_pets::set_summoned(int8_t index, game_pet_id pet_id) -> void {
	m_summoned[index] = pet_id;
}

auto player_pets::get_summoned(int8_t index) -> pet * {
	return m_summoned[index] > 0 ? m_pets[m_summoned[index]] : nullptr;
}

auto player_pets::save() -> void {
	if (m_pets.size() > 0) {
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		opt_int8_t index = 0;
		string name = "";
		int8_t level = 0;
		int16_t closeness = 0;
		int8_t fullness = 0;
		game_pet_id pet_id = 0;

		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table(vana::table::pets) << " "
			<< "SET "
			<< "	`index` = :index, "
			<< "	name = :name, "
			<< "	level = :level, "
			<< "	closeness = :closeness, "
			<< "	fullness = :fullness "
			<< "WHERE pet_id = :pet",
			soci::use(pet_id, "pet"),
			soci::use(index, "index"),
			soci::use(name, "name"),
			soci::use(level, "level"),
			soci::use(closeness, "closeness"),
			soci::use(fullness, "fullness"));

		for (const auto &kvp : m_pets) {
			pet *p = kvp.second;
			index = p->get_index();
			name = p->get_name();
			level = p->get_level();
			closeness = p->get_closeness();
			fullness = p->get_fullness();
			pet_id = p->get_id();
			st.execute(true);
		}
	}
}

auto player_pets::pet_info_packet(packet_builder &builder) -> void {
	if (auto player = m_player.lock()) {
		item *it;
		for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
			if (pet *pet = get_summoned(i)) {
				builder.add<int8_t>(1);
				builder.add<game_item_id>(pet->get_item_id());
				builder.add<string>(pet->get_name());
				builder.add<int8_t>(pet->get_level());
				builder.add<int16_t>(pet->get_closeness());
				builder.add<int8_t>(pet->get_fullness());
				builder.unk<int16_t>();
				int16_t slot = 0;
				switch (i) {
					case 0: slot = constant::equip_slot::pet_equip1;
					case 1: slot = constant::equip_slot::pet_equip2;
					case 2: slot = constant::equip_slot::pet_equip3;
				}

				it = player->get_inventory()->get_item(constant::inventory::equip, slot);
				builder.add<game_item_id>(it != nullptr ? it->get_id() : 0);
			}
		}
		builder.add<int8_t>(0); // End of pets / start of taming mob
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_pets::connect_packet(packet_builder &builder) -> void {
	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = get_summoned(i)) {
			builder.add<int64_t>(pet->get_id()); //pet->get_cash_id() != 0 ? pet->get_cash_id() : pet->get_id());
		}
		else {
			builder.add<int64_t>(0);
		}
	}
}

}
}