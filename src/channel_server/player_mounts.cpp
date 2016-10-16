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
#include "player_mounts.hpp"
#include "common/io/database.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

player_mounts::player_mounts(ref_ptr<player> player) :
	m_player{player}
{
	load();
}

auto player_mounts::save() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	if (auto player = m_player.lock()) {
		game_player_id char_id = player->get_id();
		game_item_id item_id = 0;
		int16_t exp = 0;
		uint8_t tiredness = 0;
		uint8_t level = 0;

		sql.once << "DELETE FROM " << db.make_table(vana::table::mounts) << " WHERE character_id = :char",
			soci::use(char_id, "char");

		if (m_mounts.size() > 0) {
			soci::statement st = (sql.prepare
				<< "INSERT INTO " << db.make_table(vana::table::mounts) << " "
				<< "VALUES (:char, :item, :exp, :level, :tiredness) ",
				soci::use(char_id, "char"),
				soci::use(item_id, "item"),
				soci::use(exp, "exp"),
				soci::use(level, "level"),
				soci::use(tiredness, "tiredness"));

			for (const auto &kvp : m_mounts) {
				const mount_data &c = kvp.second;
				item_id = kvp.first;
				exp = c.exp;
				level = c.level;
				tiredness = c.tiredness;
				st.execute(true);
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_mounts::load() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	if (auto player = m_player.lock()) {
		game_player_id char_id = player->get_id();

		soci::rowset<> rs = (sql.prepare << "SELECT m.* FROM " << db.make_table(vana::table::mounts) << " m WHERE m.character_id = :char ",
			soci::use(char_id, "char"));

		for (const auto &row : rs) {
			mount_data c;
			c.exp = row.get<int16_t>("exp");
			c.level = row.get<int8_t>("level");
			c.tiredness = row.get<int8_t>("tiredness");
			m_mounts[row.get<game_item_id>("mount_id")] = c;
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_mounts::get_current_exp() -> int16_t {
	return m_current_mount != 0 ? m_mounts[m_current_mount].exp : 0;
}

auto player_mounts::get_current_level() -> int8_t {
	return m_current_mount != 0 ? m_mounts[m_current_mount].level : 0;
}

auto player_mounts::get_current_tiredness() -> int8_t {
	return m_current_mount != 0 ? m_mounts[m_current_mount].tiredness : 0;
}

auto player_mounts::get_mount_exp(game_item_id id) -> int16_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].exp : 0;
}

auto player_mounts::get_mount_level(game_item_id id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].level : 0;
}

auto player_mounts::get_mount_tiredness(game_item_id id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].tiredness : 0;
}

auto player_mounts::add_mount(game_item_id id) -> void {
	mount_data c;
	c.exp = 0;
	c.level = 1;
	c.tiredness = 0;
	m_mounts[id] = c;
}

auto player_mounts::set_current_exp(int16_t exp) -> void {
	if (m_current_mount != 0) {
		mount_data c = m_mounts[m_current_mount];
		c.exp = exp;
		m_mounts[m_current_mount] = c;
	}
}

auto player_mounts::set_current_level(int8_t level) -> void {
	if (m_current_mount != 0) {
		mount_data c = m_mounts[m_current_mount];
		c.level = level;
		m_mounts[m_current_mount] = c;
	}
}

auto player_mounts::set_current_tiredness(int8_t tiredness) -> void {
	if (m_current_mount != 0) {
		mount_data c = m_mounts[m_current_mount];
		c.tiredness = tiredness;
		m_mounts[m_current_mount] = c;
	}
}

auto player_mounts::mount_info_packet(packet_builder &builder) -> void {
	if (auto player = m_player.lock()) {
		if (get_current_mount() > 0 && player->get_inventory()->get_equipped_id(constant::equip_slot::saddle) != 0) {
			builder.add<bool>(true);
			builder.add<int32_t>(get_current_level());
			builder.add<int32_t>(get_current_exp());
			builder.add<int32_t>(get_current_tiredness());
		}
		else {
			builder.add<bool>(false);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_mounts::mount_info_map_spawn_packet(packet_builder &builder) -> void {
	if (auto player = m_player.lock()) {
		if (get_current_mount() > 0 && player->get_inventory()->get_equipped_id(constant::equip_slot::saddle) != 0) {
			builder.add<int32_t>(get_current_level());
			builder.add<int32_t>(get_current_exp());
			builder.add<int32_t>(get_current_tiredness());
		}
		else {
			builder.add<int32_t>(0);
			builder.add<int32_t>(0);
			builder.add<int32_t>(0);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

}
}