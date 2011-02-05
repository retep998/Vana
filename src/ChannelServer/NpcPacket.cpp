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
#include "NpcPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketReader.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void NpcPacket::showNpc(Player *player, const NpcSpawnInfo &npc, int32_t id, bool show) {
	PacketCreator packet;
	showNpc(packet, npc, id, show);
	player->getSession()->send(packet);

	packet = PacketCreator();
	controlNpc(packet, npc, id, show);
	player->getSession()->send(packet);
}

void NpcPacket::showNpc(int32_t mapid, const NpcSpawnInfo &npc, int32_t id, bool show) {
	PacketCreator packet;
	showNpc(packet, npc, id, show);
	Maps::getMap(mapid)->sendPacket(packet);

	packet = PacketCreator();
	controlNpc(packet, npc, id, show);
	Maps::getMap(mapid)->sendPacket(packet);
}

void NpcPacket::showNpc(PacketCreator &packet, const NpcSpawnInfo &npc, int32_t id, bool show) {
	packet.addHeader(SMSG_NPC_SHOW);
	packet.add<int32_t>(id);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesRight);
	packet.add<int16_t>(npc.foothold);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
}

void NpcPacket::controlNpc(PacketCreator &packet, const NpcSpawnInfo &npc, int32_t id, bool show) {
	packet.addHeader(SMSG_NPC_CONTROL);
	packet.add<int8_t>(1);
	packet.add<int32_t>(id);
	packet.add<int32_t>(npc.id);
	packet.addPos(npc.pos);
	packet.addBool(npc.facesRight);
	packet.add<int16_t>(npc.foothold);
	packet.add<int16_t>(npc.rx0);
	packet.add<int16_t>(npc.rx1);
	packet.addBool(show);
}

void NpcPacket::animateNpc(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.addHeader(SMSG_NPC_ANIMATE);

	size_t len = pack.getBufferLength();
	if (len == 6) {
		// NPC talking
		packet.add<int32_t>(pack.get<int32_t>());
		packet.add<int16_t>(pack.get<int16_t>());
	}
	else if (len > 6) {
		// NPC moving
		packet.addBuffer(pack.getBuffer(), len - 9);
	}

	player->getSession()->send(packet);
}

void NpcPacket::showNpcEffect(Player *player, int32_t index, bool show) {
	PacketCreator packet;
	packet.addHeader(SMSG_NPC_SHOW_EFFECT);
	packet.add<int32_t>(index);
	packet.addBool(show);
	player->getSession()->send(packet);
}

void NpcPacket::showNpcEffect(int32_t mapid, int32_t index, bool show) {
	PacketCreator packet;
	packet.addHeader(SMSG_NPC_SHOW_EFFECT);
	packet.add<int32_t>(index);
	packet.addBool(show);
	Maps::getMap(mapid)->sendPacket(packet);
}

void NpcPacket::bought(Player *player, uint8_t msg) {
	PacketCreator packet;
	packet.addHeader(SMSG_ITEM_PURCHASED);
	packet.add<int8_t>(msg);
	player->getSession()->send(packet);
}
