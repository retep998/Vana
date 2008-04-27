#ifndef LOGINPACKET_H
#define LOGINPACKET_H

#define MAX_FIELD_SIZE 15

#include <vector>

using namespace std;

class PlayerLogin;
struct World;
struct Character;

namespace LoginPacket {
	void loginError(PlayerLogin* player, short errorid);
	void loginProcess(PlayerLogin* player, char id);
	void loginConnect(PlayerLogin* player, char* username, int size);
	void processOk(PlayerLogin* player);
	void showWorld(PlayerLogin* player, World world);
	void worldEnd(PlayerLogin* player);
	void showChannels(PlayerLogin* player);
	void channelSelect(PlayerLogin* player);
	void showCharacters(PlayerLogin* player, vector <Character> chars);
	void showCharacter(PlayerLogin* player, Character charc);
	void checkName(PlayerLogin* player, char is, char* name);
	void deleteCharacter(PlayerLogin* player, int ID);
	void connectIP(PlayerLogin* player, int charid);
	void logBack(PlayerLogin* player);
};

#endif