#ifndef PLAYERSPACK_H
#define PLAYERSPACK_H

class Player;
class Packet;

class PlayersPacket {
public:
	static void showMoving(Player* player, vector <Player*> players, unsigned char* packet, int size);
	static void faceExperiment(Player* player, vector <Player*> players, int face);
	static void showChat(Player* player, vector <Player*> players, char* msg);
	static void damagePlayer(Player* player, vector <Player*> players, int dmg, int mob);
	static void showMassage(char* msg, char type);
	static void showInfo(Player* player, Player* getinfo);
	static void findPlayer(Player* player, char* name, int map);
};

#endif