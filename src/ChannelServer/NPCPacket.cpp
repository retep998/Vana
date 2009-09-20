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
#include "NPCPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketReader.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void NPCPacket::showNPC(Player *player, const NPCSpawnInfo &npc, int32_t i, bool show) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_NPC);
	packet.add<int32_t>(i + 0x64);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesleft);
	packet.add<int16_t>(npc.fh);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<int16_t>(SEND_CONTROL_NPC);
	packet.add<int8_t>(1);
	packet.add<int32_t>(i + 0x64);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesleft);
	packet.add<int16_t>(npc.fh);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
	player->getSession()->send(packet);
}

void NPCPacket::showNPC(int32_t mapid, const NPCSpawnInfo &npc, int32_t i, bool show) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_NPC);
	packet.add<int32_t>(i + 0x64);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesleft);
	packet.add<int16_t>(npc.fh);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
	Maps::getMap(mapid)->sendPacket(packet);

	packet = PacketCreator();
	packet.add<int16_t>(SEND_CONTROL_NPC);
	packet.add<int8_t>(1);
	packet.add<int32_t>(i + 0x64);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesleft);
	packet.add<int16_t>(npc.fh);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
	Maps::getMap(mapid)->sendPacket(packet);
}

void NPCPacket::animateNPC(Player *player, PacketReader &pack) {
	size_t len = pack.getBufferLength();

	PacketCreator packet;
	packet.add<int16_t>(SEND_ANIMATE_NPC);
	if (len == 6) { // NPC talking
		packet.add<int32_t>(pack.get<int32_t>());
		packet.add<int16_t>(pack.get<int16_t>());
	}

	else if (len > 6) { // NPC moving
		packet.addBuffer(pack.getBuffer(), len - 9);
	}
	player->getSession()->send(packet);
}
