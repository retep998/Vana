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
#include "PlayerMonsterBook.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "ItemDataProvider.hpp"
#include "MonsterBookPacket.hpp"
#include "Player.hpp"

PlayerMonsterBook::PlayerMonsterBook(Player *player) :
	m_player(player)
{
	load();
}

auto PlayerMonsterBook::load() -> void {
	soci::session &sql = Database::getCharDb();
	player_id_t charId = m_player->getId();

	soci::rowset<> rs = (sql.prepare
		<< "SELECT b.card_id, b.level "
		<< "FROM " << Database::makeCharTable("monster_book") << " b "
		<< "WHERE b.character_id = :char "
		<< "ORDER BY b.card_id ASC",
		soci::use(charId, "char"));

	for (const auto &row : rs) {
		addCard(row.get<item_id_t>("card_id"), row.get<uint8_t>("level"), true);
	}

	calculateLevel();
}

auto PlayerMonsterBook::save() -> void {
	soci::session &sql = Database::getCharDb();
	player_id_t charId = m_player->getId();

	sql.once << "DELETE FROM " << Database::makeCharTable("monster_book") << " WHERE character_id = :char", soci::use(charId, "char");

	if (m_cards.size() > 0) {
		item_id_t cardId = 0;
		uint8_t level = 0;

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << Database::makeCharTable("monster_book") << " "
			<< "VALUES (:char, :card, :level) ",
			soci::use(charId, "char"),
			soci::use(cardId, "card"),
			soci::use(level, "level"));

		for (const auto &kvp : m_cards) {
			const MonsterCard &c = kvp.second;
			cardId = c.id;
			level = c.level;
			st.execute(true);
		}
	}
}

auto PlayerMonsterBook::getCardLevel(int32_t cardId) -> uint8_t {
	return m_cards[cardId].level;
}

auto PlayerMonsterBook::addCard(int32_t cardId, uint8_t level, bool initialLoad) -> bool {
	if (m_cards.find(cardId) == std::end(m_cards)) {
		if (GameLogicUtilities::isSpecialCard(cardId)) {
			++m_specialCount;
		}
		else {
			++m_normalCount;
		}
	}

	if (initialLoad) {
		MonsterCard card(cardId, level);
		m_cards[cardId] = card;
	}
	else {
		auto kvp = m_cards.find(cardId);
		MonsterCard card = kvp != std::end(m_cards) ? kvp->second : MonsterCard(cardId, 0);
		if (isFull(cardId)) {
			return true;
		}
		++card.level;
		m_cards[cardId] = card;
		if (card.level == 1) {
			calculateLevel();
		}
		return false;
	}
	return false;
}

auto PlayerMonsterBook::connectData(PacketBuilder &packet) -> void {
	packet.add<int32_t>(getCover() != 0 ? ChannelServer::getInstance().getItemDataProvider().getCardId(getCover()) : 0);
	packet.add<int8_t>(0);

	packet.add<uint16_t>(m_cards.size());
	for (const auto &kvp : m_cards) {
		packet.add<int16_t>(GameLogicUtilities::getCardShortId(kvp.second.id));
		packet.add<int8_t>(kvp.second.level);
	}
}

auto PlayerMonsterBook::calculateLevel() -> void {
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

auto PlayerMonsterBook::infoData(PacketBuilder &packet) -> void {
	packet.add<int32_t>(getLevel());
	packet.add<int32_t>(getNormals());
	packet.add<int32_t>(getSpecials());
	packet.add<int32_t>(getSize());
	packet.add<int32_t>(getCover());
}

auto PlayerMonsterBook::getCard(int32_t cardId) -> MonsterCard * {
	auto kvp = m_cards.find(cardId);
	return kvp != std::end(m_cards) ? &kvp->second : nullptr;
}

auto PlayerMonsterBook::isFull(int32_t cardId) -> bool {
	auto kvp = m_cards.find(cardId);
	return kvp != std::end(m_cards) ? (kvp->second.level == MonsterCards::MaxCardLevel) : false;
}