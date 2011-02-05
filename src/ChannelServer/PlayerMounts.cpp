/*
Copyright (C) 2008-2011 Vana Development Team

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

PlayerMounts::PlayerMounts(Player *p) : m_player(p), m_currentmount(0) {
	load();
}

void PlayerMounts::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM mounts WHERE charid = " << m_player->getId();
	query.exec();

	if (m_mounts.size() > 0) {
		bool firstrun = true;
		MountData c;
		for (unordered_map<int32_t, MountData>::iterator iter = m_mounts.begin(); iter != m_mounts.end(); iter++) {
			c = iter->second;
			if (firstrun) {
				query << "INSERT INTO mounts VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << m_player->getId() << ","
					<< iter->first << ","
					<< c.exp << ","
					<< (int16_t) c.level << ","
					<< (int16_t) c.tiredness << ")";
		}
		query.exec();
	}
}

void PlayerMounts::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM mounts WHERE charid = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();
	MountData c;
	for (size_t i = 0; i < res.size(); i++) {
		c.exp = (int16_t) res[i]["exp"];
		c.level = (int8_t) res[i]["level"];
		c.tiredness = (int8_t) res[i]["tiredness"];
		m_mounts[res[i]["mountid"]] = c;
	}
}

int16_t PlayerMounts::getCurrentExp() {
	return (m_currentmount != 0 ? m_mounts[m_currentmount].exp : 0);
}

int8_t PlayerMounts::getCurrentLevel() {
	return (m_currentmount != 0 ? m_mounts[m_currentmount].level : 0);
}

int8_t PlayerMounts::getCurrentTiredness() {
	return (m_currentmount != 0 ? m_mounts[m_currentmount].tiredness : 0);
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
	if (m_currentmount != 0) {
		MountData c = m_mounts[m_currentmount];
		c.exp = exp;
		m_mounts[m_currentmount] = c;
	}
}

void PlayerMounts::setCurrentLevel(int8_t level) {
	if (m_currentmount != 0) {
		MountData c = m_mounts[m_currentmount];
		c.level = level;
		m_mounts[m_currentmount] = c;
	}
}

void PlayerMounts::setCurrentTiredness(int8_t tiredness) {
	if (m_currentmount != 0) {
		MountData c = m_mounts[m_currentmount];
		c.tiredness = tiredness;
		m_mounts[m_currentmount] = c;
	}
}

void PlayerMounts::mountInfoPacket(PacketCreator &packet) {
	if (getCurrentMount() > 0 && m_player->getInventory()->getEquippedId(EquipSlots::Saddle) != 0) {
		packet.addBool(true);
		packet.add<int32_t>(getCurrentLevel());
		packet.add<int32_t>(getCurrentExp());
		packet.add<int32_t>(getCurrentTiredness());
	}
	else {
		packet.addBool(false);
	}
}