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
#include "MonsterBookPacket.hpp"
#include "ItemDataProvider.hpp"
#include "Maps.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "SmsgHeader.hpp"

auto MonsterBookPacket::addCard(Player *player, int32_t cardId, uint8_t level, bool full) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MONSTER_BOOK_ADD);
	packet.add<bool>(!full);
	packet.add<int32_t>(cardId);
	packet.add<int32_t>(level);
	player->getSession()->send(packet);

	if (!full) {
		packet = PacketCreator();
		packet.add<header_t>(SMSG_THEATRICS);
		packet.add<int8_t>(0x0D);
		player->getSession()->send(packet);

		// GMS doesnt send the animation for others.
		// If you want to enable displaying it, just uncomment this VV
		/*
		packet = PacketCreator();
		packet.add<header_t>(SMSG_SKILL_SHOW);
		packet.add<int32_t>(player->getId());
		packet.add<int8_t>(0x0D);
		player->getMap()->sendPacket(packet, player);
		*/
	}
}

auto MonsterBookPacket::changeCover(Player *player, int32_t cardId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MONSTER_BOOK_COVER);
	packet.add<int32_t>(cardId);
	player->getSession()->send(packet);
}