#ifndef SERVERPACK_H
#define SERVERPACK_H

#include <vector>
#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

namespace ServerPacket {
	void showScrollingHeader(Player* player, char* msg);
	void changeScrollingHeader(char* msg);
	void scrollingHeaderOff();
};

#endif