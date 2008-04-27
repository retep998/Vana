#ifndef FAMEPACK_H
#define FAMEPACK_H

#include <vector>

using namespace std;
class Player;
class Packet;

namespace FamePacket{
	void SendFame(Player* player, Player* player2, char* cFamer, int charLen, int FameDefame, int NewFame);
	void SendError(Player* player, int reason);
	void UpdateFame(Player* player);
};
#endif