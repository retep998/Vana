#ifndef MAPPACK_H
#define MAPPACK_H

class Player;
class Packet;

class MapPacket {
public:
	static Packet playerPacket(Player* player);
	static void showPlayer(Player* player, vector <Player*> players);
	static void showPlayers(Player* player, vector <Player*> players);
	static void removePlayer(Player* player, vector <Player*> players);
	static void changeMap(Player* player);
	static void makeApple(Player* player);
};

#endif