#ifndef SERVERPACK_H
#define SERVERPACK_H

#include <vector>
#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

class ServerPacket {
public:
	static void showScrollingHeader(Player* player, char* msg);
	static void changeScrollingHeader(char* msg);
	static void scrollingHeaderOff();
};

#endif