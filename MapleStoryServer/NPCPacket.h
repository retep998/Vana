#ifndef NPCPACK_H
#define NPCPACK_H

class Player;
struct NPCInfo;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class NPCPacket {
public:
	static void showNPC(Player* player, NPCInfo npc, int i);
};

#endif