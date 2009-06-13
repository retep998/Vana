/*
Copyright (C) 2008-2009 Vana Development Team

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

PlayerMonsterBook::PlayerMonsterBook(Player *player) : m_player(player), m_cover(0), m_specialcount(0), m_normalcount(0), m_level(1) { 

}

void PlayerMonsterBook::connectData(PacketCreator &packet) {
	packet.add<int32_t>(0); // Cover
	packet.add<int8_t>(0);
	packet.add<int16_t>(0); // Card count
}

void PlayerMonsterBook::infoData(PacketCreator &packet) {
	packet.add<int32_t>(getLevel());
	packet.add<int32_t>(getNormals());
	packet.add<int32_t>(getSpecials()); 
	packet.add<int32_t>(getSize());
	packet.add<int32_t>(getCover());
}