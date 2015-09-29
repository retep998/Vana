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
#include "PlayerMounts.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "Player.hpp"

namespace Vana {

PlayerMounts::PlayerMounts(Player *player) :
	m_player{player}
{
	load();
}

auto PlayerMounts::save() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();
	item_id_t itemId = 0;
	int16_t exp = 0;
	uint8_t tiredness = 0;
	uint8_t level = 0;

	sql.once << "DELETE FROM " << db.makeTable("mounts") << " WHERE character_id = :char", soci::use(charId, "char");

	if (m_mounts.size() > 0) {
		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.makeTable("mounts") << " "
			<< "VALUES (:char, :item, :exp, :level, :tiredness) ",
			soci::use(charId, "char"),
			soci::use(itemId, "item"),
			soci::use(exp, "exp"),
			soci::use(level, "level"),
			soci::use(tiredness, "tiredness"));

		for (const auto &kvp : m_mounts) {
			const MountData &c = kvp.second;
			itemId = kvp.first;
			exp = c.exp;
			level = c.level;
			tiredness = c.tiredness;
			st.execute(true);
		}
	}
}

auto PlayerMounts::load() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();

	soci::rowset<> rs = (sql.prepare << "SELECT m.* FROM " << db.makeTable("mounts") << " m WHERE m.character_id = :char ", soci::use(charId, "char"));

	for (const auto &row : rs) {
		MountData c;
		c.exp = row.get<int16_t>("exp");
		c.level = row.get<int8_t>("level");
		c.tiredness = row.get<int8_t>("tiredness");
		m_mounts[row.get<item_id_t>("mount_id")] = c;
	}
}

auto PlayerMounts::getCurrentExp() -> int16_t {
	return m_currentMount != 0 ? m_mounts[m_currentMount].exp : 0;
}

auto PlayerMounts::getCurrentLevel() -> int8_t {
	return m_currentMount != 0 ? m_mounts[m_currentMount].level : 0;
}

auto PlayerMounts::getCurrentTiredness() -> int8_t {
	return m_currentMount != 0 ? m_mounts[m_currentMount].tiredness : 0;
}

auto PlayerMounts::getMountExp(item_id_t id) -> int16_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].exp : 0;
}

auto PlayerMounts::getMountLevel(item_id_t id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].level : 0;
}

auto PlayerMounts::getMountTiredness(item_id_t id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].tiredness : 0;
}

auto PlayerMounts::addMount(item_id_t id) -> void {
	MountData c;
	c.exp = 0;
	c.level = 1;
	c.tiredness = 0;
	m_mounts[id] = c;
}

auto PlayerMounts::setCurrentExp(int16_t exp) -> void {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.exp = exp;
		m_mounts[m_currentMount] = c;
	}
}

auto PlayerMounts::setCurrentLevel(int8_t level) -> void {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.level = level;
		m_mounts[m_currentMount] = c;
	}
}

auto PlayerMounts::setCurrentTiredness(int8_t tiredness) -> void {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.tiredness = tiredness;
		m_mounts[m_currentMount] = c;
	}
}

auto PlayerMounts::mountInfoPacket(PacketBuilder &builder) -> void {
	if (getCurrentMount() > 0 && m_player->getInventory()->getEquippedId(EquipSlots::Saddle) != 0) {
		builder.add<bool>(true);
		builder.add<int32_t>(getCurrentLevel());
		builder.add<int32_t>(getCurrentExp());
		builder.add<int32_t>(getCurrentTiredness());
	}
	else {
		builder.add<bool>(false);
	}
}

auto PlayerMounts::mountInfoMapSpawnPacket(PacketBuilder &builder) -> void {
	if (getCurrentMount() > 0 && m_player->getInventory()->getEquippedId(EquipSlots::Saddle) != 0) {
		builder.add<int32_t>(getCurrentLevel());
		builder.add<int32_t>(getCurrentExp());
		builder.add<int32_t>(getCurrentTiredness());
	}
	else {
		builder.add<int32_t>(0);
		builder.add<int32_t>(0);
		builder.add<int32_t>(0);
	}
}

}