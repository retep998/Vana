#include "NPCPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "NPCs.h"

void NPCPacket::showNPC(Player* player, NPCInfo npc, int i){
	Packet packet = Packet();
	packet.addHeader(0xA7);
	packet.addInt(i+0x64);
	packet.addInt(npc.id);
	packet.addShort(npc.x);
	packet.addShort(npc.cy);
	packet.addByte(1);
	packet.addShort(npc.fh);
	packet.addShort(npc.rx0);
	packet.addShort(npc.rx1);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0xAC);
	packet.addByte(1);
	packet.addInt(i+0x64);
	packet.addInt(npc.id);
	packet.addShort(npc.x);
	packet.addShort(npc.cy);
	packet.addByte(1);
	packet.addShort(npc.fh);
	packet.addShort(npc.rx0);
	packet.addShort(npc.rx1);
	packet.packetSend(player);
}