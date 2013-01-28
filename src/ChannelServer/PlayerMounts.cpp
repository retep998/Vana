/*
Copyright (C) 2008-2013 Vana Development Team

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

PlayerMounts::PlayerMounts(Player *p) : m_player(p), m_currentMount(0) {
	load();
}

void PlayerMounts::save() {
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

		for (unordered_map<int32_t, MountData>::iterator iter = m_mounts.begin(); iter != m_mounts.end(); ++iter) {
			MountData &c = iter->second;
			itemId = iter->first;
			exp = c.exp;
			level = c.level;
			tiredness = c.tiredness;
			st.execute(true);
		}
	}
}

void PlayerMounts::load() {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	MountData c;

	soci::rowset<> rs = (sql.prepare << "SELECT m.* FROM mounts m WHERE m.character_id = :char ", soci::use(charId, "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		const soci::row &row = *i;

		c = MountData();
		c.exp = row.get<int16_t>("exp");
		c.level = row.get<int8_t>("level");
		c.tiredness = row.get<int8_t>("tiredness");
		m_mounts[row.get<int32_t>("mount_id")] = c;
	}
}

int16_t PlayerMounts::getCurrentExp() {
	return (m_currentMount != 0 ? m_mounts[m_currentMount].exp : 0);
}

int8_t PlayerMounts::getCurrentLevel() {
	return (m_currentMount != 0 ? m_mounts[m_currentMount].level : 0);
}

int8_t PlayerMounts::getCurrentTiredness() {
	return (m_currentMount != 0 ? m_mounts[m_currentMount].tiredness : 0);
}

int16_t PlayerMounts::getMountExp(int32_t id) {
	return (m_mounts.find(id) != m_mounts.end() ? m_mounts[id].exp : 0);
}

int8_t PlayerMounts::getMountLevel(int32_t id) {
	return (m_mounts.find(id) != m_mounts.end() ? m_mounts[id].level : 0);
}

int8_t PlayerMounts::getMountTiredness(int32_t id) {
	return (m_mounts.find(id) != m_mounts.end() ? m_mounts[id].tiredness : 0);
}

void PlayerMounts::addMount(int32_t id) {
	MountData c;
	c.exp = 0;
	c.level = 1;
	c.tiredness = 0;
	m_mounts[id] = c;
}

void PlayerMounts::setCurrentExp(int16_t exp) {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.exp = exp;
		m_mounts[m_currentMount] = c;
	}
}

void PlayerMounts::setCurrentLevel(int8_t level) {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.level = level;
		m_mounts[m_currentMount] = c;
	}
}

void PlayerMounts::setCurrentTiredness(int8_t tiredness) {
	if (m_currentMount != 0) {
		MountData c = m_mounts[m_currentMount];
		c.tiredness = tiredness;
		m_mounts[m_currentMount] = c;
	}
}

void PlayerMounts::mountInfoPacket(PacketCreator &packet) {
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