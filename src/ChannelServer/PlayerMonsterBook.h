/*
Copyright (C) 2008-2012 Vana Development Team

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

#pragma once

#include "noncopyable.hpp"
#include "Types.h"
#include <unordered_map>

using std::unordered_map;

class Player;
class PacketCreator;

struct MonsterCard {
	MonsterCard() : id(0), level(0) { }
	MonsterCard(int32_t id, uint8_t level) : id(id), level(level) { }
	int32_t id; // Card ID
	uint8_t level; // 1 - 5
};

class PlayerMonsterBook : boost::noncopyable {
public:
	PlayerMonsterBook(Player *player);

	void load();
	void save();
	void connectData(PacketCreator &packet);

	bool addCard(int32_t itemId, uint8_t level = 1, bool initialLoad = false);
	void calculateLevel();
	void infoData(PacketCreator &packet);
	void setCover(int32_t newCover) { m_cover = newCover; }

	MonsterCard * getCard(int32_t cardId);
	uint8_t getCardLevel(int32_t cardId);
	int32_t getSpecials() const { return m_specialCount; }
	int32_t getNormals() const { return m_normalCount; }
	int32_t getSize() const { return (int32_t) m_cards.size(); }
	int32_t getLevel() const { return m_level; }
	int32_t getCover() const { return m_cover; }
	bool isFull(int32_t cardId);
private:
	int32_t m_specialCount;
	int32_t m_normalCount;
	int32_t m_level;
	int32_t m_cover;
	Player *m_player;
	unordered_map<int32_t, MonsterCard> m_cards;
};