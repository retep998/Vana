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
#include "MonsterBookPacket.h"
#include "ItemDataProvider.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void MonsterBookPacket::addCard(Player *player, int32_t cardid, uint8_t level, bool full) {
	PacketCreator packet;
	packet.addHeader(SMSG_MONSTER_BOOK_ADD);
	packet.addBool(!full);
	packet.add<int32_t>(cardid);
	packet.add<int32_t>(level);
	player->getSession()->send(packet);

	if (!full) {
		packet = PacketCreator();
		packet.addHeader(SMSG_THEATRICS);
		packet.add<int8_t>(0x0D);
		player->getSession()->send(packet);

		// GMS doesn't send the animation for others.
		// If you want to enable displaying it, just uncomment this VV
		/*
		packet = PacketCreator();
		packet.addHeader(SMSG_SKILL_SHOW);
		packet.add<int32_t>(player->getId());
		packet.add<int8_t>(0x0D);
		Maps::getMap(player->getMap())->sendPacket(packet, player);
		*/
	}
}

void MonsterBookPacket::changeCover(Player *player, int32_t cardid) {
	PacketCreator packet;
	packet.addHeader(SMSG_MONSTER_BOOK_COVER);
	packet.add<int32_t>(cardid);
	player->getSession()->send(packet);
}
