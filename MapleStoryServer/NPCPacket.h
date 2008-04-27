#ifndef NPCPACK_H
#define NPCPACK_H

class Player;
struct NPCInfo;

namespace NPCPacket {
	void showNPC(Player* player, NPCInfo npc, int i);
};

#endif