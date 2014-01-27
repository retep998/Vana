/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "PlayerMounts.h"
#include "Database.h"
#include "GameConstants.h"
#include "PacketCreator.h"
#include "Player.h"

PlayerMounts::PlayerMounts(Player *p) :
	m_player(p)
{
	load();
}

auto PlayerMounts::save() -> void {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	int32_t itemId = 0;
	int16_t exp = 0;
	uint8_t tiredness = 0;
	uint8_t level = 0;

	sql.once << "DELETE FROM mounts WHERE character_id = :char", soci::use(charId, "char");

	if (m_mounts.size() > 0) {
		soci::statement st = (sql.prepare
			<< "INSERT INTO mounts "
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
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	MountData c;

	soci::rowset<> rs = (sql.prepare << "SELECT m.* FROM mounts m WHERE m.character_id = :char ", soci::use(charId, "char"));

	for (const auto &row : rs) {
		c = MountData();
		c.exp = row.get<int16_t>("exp");
		c.level = row.get<int8_t>("level");
		c.tiredness = row.get<int8_t>("tiredness");
		m_mounts[row.get<int32_t>("mount_id")] = c;
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

auto PlayerMounts::getMountExp(int32_t id) -> int16_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].exp : 0;
}

auto PlayerMounts::getMountLevel(int32_t id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].level : 0;
}

auto PlayerMounts::getMountTiredness(int32_t id) -> int8_t {
	return m_mounts.find(id) != std::end(m_mounts) ? m_mounts[id].tiredness : 0;
}

auto PlayerMounts::addMount(int32_t id) -> void {
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

auto PlayerMounts::mountInfoPacket(PacketCreator &packet) -> void {
	if (getCurrentMount() > 0 && m_player->getInventory()->getEquippedId(EquipSlots::Saddle) != 0) {
		packet.add<bool>(true);
		packet.add<int32_t>(getCurrentLevel());
		packet.add<int32_t>(getCurrentExp());
		packet.add<int32_t>(getCurrentTiredness());
	}
	else {
		packet.add<bool>(false);
	}
}