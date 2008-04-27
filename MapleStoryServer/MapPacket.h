#ifndef MAPPACK_H
#define MAPPACK_H

class Player;
class Packet;

namespace MapPacket {
	Packet playerPacket(Player* player);
	void showPlayer(Player* player, vector <Player*> players);
	void showPlayers(Player* player, vector <Player*> players);
	void removePlayer(Player* player, vector <Player*> players);
	void changeMap(Player* player);
	void makeApple(Player* player);
};

#endif