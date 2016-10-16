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
#include "pet.hpp"
#include "common/data/provider/item.hpp"
#include "common/io/database.hpp"
#include "common/timer/timer.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/pets_packet.hpp"
#include "channel_server/player.hpp"
#include <functional>

namespace vana {
namespace channel_server {

pet::pet(player *player, item *item) :
	movable_life{0, point{}, 0},
	m_player{player},
	m_item_id{item->get_id()},
	m_name{channel_server::get_instance().get_item_data_provider().get_item_info(m_item_id)->name},
	m_item{item}
{
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();

	sql.once << "INSERT INTO " << db.make_table(vana::table::pets) << " (name) VALUES (:name)",
		soci::use(m_name, "name");

	m_id = db.get_last_id<game_pet_id>();
	item->set_pet_id(m_id);
}

pet::pet(player *player, item *item, const soci::row &row) :
	movable_life{0, point{}, 0},
	m_player{player},
	m_id{item->get_pet_id()},
	m_item_id{item->get_id()},
	m_item{item}
{
	initialize_pet(row);
	if (is_summoned()) {
		if (m_index.is_initialized() && m_index.get() == 1) {
			start_timer();
		}
		player->get_pets()->set_summoned(m_index.get(), m_id);
	}
}

auto pet::level_up() -> void {
	m_level += 1;
	m_player->send_map(packets::pets::level_up(m_player->get_id(), this));
}

auto pet::set_name(const string &name) -> void {
	m_name = name;
	m_player->send_map(packets::pets::change_name(m_player->get_id(), this));
}

auto pet::add_closeness(int16_t amount) -> void {
	m_closeness += amount;
	if (m_closeness > constant::stat::max_closeness) {
		m_closeness = constant::stat::max_closeness;
	}
	while (m_closeness >= constant::stat::pet_exp[m_level - 1] && m_level < constant::stat::pet_levels) {
		level_up();
	}

	m_player->send(packets::pets::update_pet(this, m_item));
}

auto pet::modify_fullness(int8_t offset, bool send_packet) -> void {
	if (m_fullness + offset > constant::stat::max_fullness) {
		m_fullness = constant::stat::max_fullness;
	}
	else {
		m_fullness += offset;
	}

	if (m_fullness < constant::stat::min_fullness) {
		m_fullness = constant::stat::min_fullness;
	}

	if (send_packet) {
		m_player->send(packets::pets::update_pet(this, m_item));
	}
}

auto pet::start_timer() -> void {
	vana::timer::id id{vana::timer::type::pet_timer, get_index().get()}; // The timer will automatically stop if another pet gets inserted into this index
	duration repeat = seconds{(6 - channel_server::get_instance().get_item_data_provider().get_pet_info(get_item_id())->hunger) * 60}; // TODO FIXME formula
	vana::timer::timer::create(
		[this](const time_point &now) {
			this->modify_fullness(-1, true);
		},
		id,
		m_player->get_timer_container(),
		seconds{0},
		repeat);
}

auto pet::has_name_tag() const -> bool {
	if (m_index.is_initialized()) {
		switch (m_index.get()) {
			case 0: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_label_ring1, true) != 0;
			case 1: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_label_ring2, true) != 0;
			case 2: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_label_ring3, true) != 0;
		}
	}
	return false;
}

auto pet::has_quote_item() const -> bool {
	if (m_index.is_initialized()) {
		switch (m_index.get()) {
			case 0: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_quote_ring1, true) != 0;
			case 1: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_quote_ring2, true) != 0;
			case 2: return m_player->get_inventory()->get_equipped_id(constant::equip_slot::pet_quote_ring3, true) != 0;
		}
	}
	return false;
}

auto pet::initialize_pet(const soci::row &row) -> void {
	m_index = row.get<opt_int8_t>("index");
	m_name = row.get<string>("pet_name");
	m_level = row.get<int8_t>("level");
	m_closeness = row.get<int16_t>("closeness");
	m_fullness = row.get<int8_t>("fullness");
	m_inventory_slot = row.get<int8_t>("slot");
}

}
}