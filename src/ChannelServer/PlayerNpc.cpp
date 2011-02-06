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
#include "PlayerNpc.h"
#include "Database.h"
#include "ItemConstants.h"
#include "Player.h"
#include "SendHeader.h"

PlayerNpc::PlayerNpc(int32_t npcId, int32_t charid, uint8_t level, int8_t gender, int8_t skin, int32_t eyes, int32_t hair) :
npcId(npcId),
characterId(charid),
level(level),
gender(gender),
skin(skin),
eyes(eyes),
hair(hair)
{
	loadEquips();

	// Set name (done for name changes)
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT name FROM characters WHERE ID = " << characterId;
	mysqlpp::StoreQueryResult res = query.store();
	res[0][0].to_string(name);
}

void PlayerNpc::loadEquips() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM imitate_items WHERE charid = " << characterId;
	mysqlpp::UseQueryResult res = query.use();

	enum TableFields {
		ItemCharId = 0,
		Slot, ItemId
	};
	
	int16_t slot;
	while (MYSQL_ROW row = res.fetch_raw_row()) {
		slot = abs(atoi(row[Slot]));
		if (slot == EquipSlots::Mount)
			continue;

		if (slot > 100) // Cash items
			m_equipped[slot - 100][1] = atoi(row[ItemId]);
		else // Normal items
			m_equipped[slot][0] = atoi(row[ItemId]);
	}
}

void PlayerNpc::renewData(Player *player) {
	m_equipped = player->getInventory()->getEquippedData();
	skin = player->getSkin();
	eyes = player->getEyes();
	hair = player->getHair();
	save();

	makePacket(player);
}

void PlayerNpc::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE imitate_characters SET "
		<< "skin = " << (int16_t)skin << ", "
		<< "eyes = " << eyes << ", "
		<< "hair = " << hair << " "
		<< "WHERE charid = " << characterId;
	query.exec();

	query << "DELETE FROM imitate_items WHERE charid = " << characterId;
	query.exec();

	bool firstRun = true;
	int16_t slot;
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) {
		for (int8_t j = 0; j < 2; j++) {
			if (m_equipped[i][j] > 0) {
				slot = i;
				if (j == 1) {
					// It's a cash item
					slot += 100;
				}
				if (firstRun) {
					query << "INSERT INTO imitate_items VALUES (";
					firstRun = false;
				}
				else {
					query << ", (";
				}
				query << characterId << ", " << slot  << ", " << m_equipped[i][j] << ")";
			}
		}
	}
	if (!firstRun) {
		query.exec();
	}
}

void PlayerNpc::makePacket(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_NPC_SHOW);
	packet.add<int8_t>(1);
	makePacket(packet);

	player->getSession()->send(packet);
}

void PlayerNpc::makePacket(PacketCreator &packet) {
	packet.add<int32_t>(npcId);
	packet.addString(name);

	packet.add<int8_t>(gender);
	packet.add<int8_t>(skin);
	packet.add<int32_t>(eyes);
	packet.add<int8_t>(1);
	packet.add<int32_t>(hair);

	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Shown items
		if (m_equipped[i][0] > 0 || m_equipped[i][1] > 0) {
			packet.add<int8_t>(i);
			if (m_equipped[i][1] <= 0 || (i == 11 && m_equipped[i][0] > 0)) // Normal weapons always here
				packet.add<int32_t>(m_equipped[i][0]);
			else
				packet.add<int32_t>(m_equipped[i][1]);
		}
	}
	packet.add<int8_t>(-1);

	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Covered items
		if (m_equipped[i][1] > 0 && m_equipped[i][0] > 0 && i != 11) {
			packet.add<int8_t>(i);
			packet.add<int32_t>(m_equipped[i][0]);
		}
	}
	packet.add<int8_t>(-1);
	packet.add<int32_t>(m_equipped[11][1]); // Cash weapon


	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		packet.add<int32_t>(0);
	}
}