#ifndef PLAYERSPACK_H
#define PLAYERSPACK_H

class Player;
class Packet;

namespace PlayersPacket {
	void showMoving(Player* player, vector <Player*> players, unsigned char* packet, int size);
	void faceExperiment(Player* player, vector <Player*> players, int face);
	void showChat(Player* player, vector <Player*> players, char* msg);
	void damagePlayer(Player* player, vector <Player*> players, int dmg, int mob);
	void showMassage(char* msg, char type);
	void showInfo(Player* player, Player* getinfo);
	void findPlayer(Player* player, char* name, int map, unsigned char is = 0);
	void whisperPlayer(Player* player, Player* target, char* message);
};

#endif