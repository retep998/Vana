#ifndef SERVERPACK_H
#define SERVERPACK_H

#include <vector>
#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class ServerPacket {
public:
	static void showScrollingHeader(Player* player, char* msg);
	static void changeScrollingHeader(char* msg);
	static void scrollingHeaderOff();
};

#endif