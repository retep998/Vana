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

#pragma once

#include "Types.h"
#include <unordered_map>

class PacketCreator;
class Player;

struct MonsterCard {
	MonsterCard() = default;
	MonsterCard(int32_t id, uint8_t level) : id(id), level(level) { }

	int32_t id = 0;
	uint8_t level = 0;
};

class PlayerMonsterBook {
	NONCOPYABLE(PlayerMonsterBook);
	NO_DEFAULT_CONSTRUCTOR(PlayerMonsterBook);
public:
	PlayerMonsterBook(Player *player);

	auto load() -> void;
	auto save() -> void;
	auto connectData(PacketCreator &packet) -> void;

	auto addCard(int32_t itemId, uint8_t level = 1, bool initialLoad = false) -> bool;
	auto calculateLevel() -> void;
	auto infoData(PacketCreator &packet) -> void;
	auto setCover(int32_t newCover) -> void { m_cover = newCover; }

	auto getCard(int32_t cardId) -> MonsterCard *;
	auto getCardLevel(int32_t cardId) -> uint8_t;
	auto getSpecials() const -> int32_t { return m_specialCount; }
	auto getNormals() const -> int32_t { return m_normalCount; }
	auto getSize() const -> int32_t { return static_cast<int32_t>(m_cards.size()); }
	auto getLevel() const -> int32_t { return m_level; }
	auto getCover() const -> int32_t { return m_cover; }
	auto isFull(int32_t cardId) -> bool;
private:
	int32_t m_specialCount = 0;
	int32_t m_normalCount = 0;
	int32_t m_level = 1;
	int32_t m_cover = 0;
	Player *m_player = nullptr;
	hash_map_t<int32_t, MonsterCard> m_cards;
};