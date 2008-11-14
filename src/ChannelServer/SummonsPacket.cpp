/*
Copyright (C) 2008 Vana Development Team

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
#include "SummonsPacket.h"
#include "Summons.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "SendHeader.h"

void SummonsPacket::showSummon(Player *player, Summon *summon, bool animated, Player *toplayer) {
	PacketCreator packet;
	packet.addShort(SEND_SPAWN_SUMMON);
	packet.addInt(player->getId());
	packet.addInt(summon->getID());
	packet.addInt(summon->getSummonID());
	packet.addByte(summon->getLevel());
	packet.addPos(summon->getPos());
	packet.addByte(4); // ?
	packet.addByte(0x53); // ?
	packet.addByte(1); // ?
	packet.addByte(summon->getType()); // Movement type
	packet.addByte(!ISPUPPET(summon->getSummonID())); // Attack or not
	packet.addByte(!animated);
	if (toplayer != 0)
		toplayer->getSession()->send(packet);
	else
		Maps::getMap(player->getMap())->sendPacket(packet);
}

void SummonsPacket::moveSummon(Player *player, Summon *summon, Pos &startPos, unsigned char *buf, int32_t buflen) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_SUMMON);
	packet.addInt(player->getId());
	packet.addInt(summon->getID());
	packet.addPos(startPos);
	packet.addBuffer(buf, buflen);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SummonsPacket::removeSummon(Player *player, Summon *summon, bool animated) {
	PacketCreator packet;
	packet.addShort(SEND_REMOVE_SUMMON);
	packet.addInt(player->getId());
	packet.addInt(summon->getID());
	packet.addByte(animated ? 4 : 1); // ?
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void SummonsPacket::damageSummon(Player *player, int32_t summonid, int8_t notsure, int32_t damage, int32_t mobid) {
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_SUMMON);
	packet.addInt(player->getId());
	packet.addInt(summonid);
	packet.addByte(notsure);
	packet.addInt(damage);
	packet.addInt(mobid);
	packet.addByte(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}
