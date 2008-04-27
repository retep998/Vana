#ifndef NPCPACK_H
#define NPCPACK_H

class Player;
struct NPCInfo;

class NPCPacket {
public:
	static void showNPC(Player* player, NPCInfo npc, int i);
};

#endif