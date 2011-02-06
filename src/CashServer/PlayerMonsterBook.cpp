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
#include "PlayerMonsterBook.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "ItemDataProvider.h"
#include "PacketCreator.h"
#include "Player.h"

PlayerMonsterBook::PlayerMonsterBook(Player *player) : m_player(player),  m_specialcount(0),  m_normalcount(0), m_level(1) { 
	load(); 
}

void PlayerMonsterBook::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT cardid, level FROM monsterbook WHERE charid = " << m_player->getId() << " ORDER BY cardid ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		addCard(res[i][0], (uint8_t) res[i][1], true);
	}

	calculateLevel();
}

void PlayerMonsterBook::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "DELETE FROM monsterbook WHERE charid = " << m_player->getId();
	query.exec();

	bool firstrun = true;
	for (unordered_map<int32_t, MonsterCard>::iterator iter = m_cards.begin(); iter != m_cards.end(); iter++) {
		if (firstrun) {
			query << "INSERT INTO monsterbook VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << m_player->getId() << ","
			<< iter->second.id << ","
			<< static_cast<int16_t>(iter->second.level) << ")";
	}
	if (!firstrun)
		query.exec();
}

uint8_t PlayerMonsterBook::getCardLevel(int32_t cardid) {
	return m_cards[cardid].level;
}

bool PlayerMonsterBook::addCard(int32_t cardid, uint8_t level, bool initialload) {
	if (m_cards.find(cardid) == m_cards.end()) {
		if (GameLogicUtilities::isSpecialCard(cardid)) {
			m_specialcount++;
		}
		else {
			m_normalcount++;
		}
	}

	if (initialload) {
		MonsterCard card = MonsterCard(cardid, level);
		m_cards[cardid] = card;
	}
	else {
		MonsterCard card = (m_cards.find(cardid) != m_cards.end() ? m_cards[cardid] : MonsterCard(cardid, 0));
		if (isFull(cardid)) {
			return true;
		}
		card.level++;
		m_cards[cardid] = card;
		if (card.level == 1) {
			calculateLevel();
		}
		return false;
	}
	return false;
}

void PlayerMonsterBook::connectData(PacketCreator &packet) {
	packet.add<int32_t>(getCover() != 0 ? ItemDataProvider::Instance()->getCardId(getCover()) : 0);
	packet.add<int8_t>(0);
	
	packet.add<uint16_t>(m_cards.size());
	for (unordered_map<int32_t, MonsterCard>::iterator iter = m_cards.begin(); iter != m_cards.end(); iter++) {
		packet.add<int16_t>(GameLogicUtilities::getCardShortId(iter->second.id));
		packet.add<int8_t>(iter->second.level);
	}
}

void PlayerMonsterBook::calculateLevel() {
	int32_t size = getSize();
	m_level = MonsterCards::MaxPlayerLevel;
	for (int32_t i = 1; i < MonsterCards::MaxPlayerLevel; i++) {
		// We don't calculate for the last level because that includes all values above the second to last level
		if (size < MonsterCards::PlayerLevels[i - 1]) {
			m_level = i;
			break;
		}
	}
}

void PlayerMonsterBook::infoData(PacketCreator &packet) {
	packet.add<int32_t>(getLevel());
	packet.add<int32_t>(getNormals());
	packet.add<int32_t>(getSpecials());
	packet.add<int32_t>(getSize());
	packet.add<int32_t>(getCover());
}

MonsterCard * PlayerMonsterBook::getCard(int32_t cardid) {
	return (m_cards.find(cardid) != m_cards.end() ? &m_cards[cardid] : nullptr);
}

bool PlayerMonsterBook::isFull(int32_t cardid) {
	return (m_cards.find(cardid) != m_cards.end() ? (m_cards[cardid].level == MonsterCards::MaxCardLevel) : false);
}
