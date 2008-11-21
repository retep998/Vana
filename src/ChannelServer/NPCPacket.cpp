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
#include "NPCPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketReader.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void NPCPacket::showNPC(Player *player, NPCSpawnInfo npc, int32_t i) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_NPC);
	packet.addInt(i+0x64);
	packet.addInt(npc.id);
	packet.addPos(npc.pos);
	packet.addByte(1);
	packet.addShort(npc.fh);
	packet.addShort(npc.rx0);
	packet.addShort(npc.rx1);
	packet.addByte(1);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_CONTROL_NPC);
	packet.addByte(1);
	packet.addInt(i+0x64);
	packet.addInt(npc.id);
	packet.addPos(npc.pos);
	packet.addByte(1);
	packet.addShort(npc.fh);
	packet.addShort(npc.rx0);
	packet.addShort(npc.rx1);
	packet.addByte(1);
	player->getSession()->send(packet);
}

void NPCPacket::animateNPC(Player *player, PacketReader &pack) {
	size_t len = pack.getBufferLength();

	PacketCreator packet;
	packet.addShort(SEND_ANIMATE_NPC);
	if (len == 6) { // NPC talking
		packet.addInt(pack.getInt());
		packet.addShort(pack.getShort());
	}

	else if (len > 6) { // NPC moving
		packet.addBuffer(pack.getBuffer(), len - 9);
	}
	player->getSession()->send(packet);
}
