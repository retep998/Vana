#ifndef LOGINPACKET_H
#define LOGINPACKET_H

#define MAX_FIELD_SIZE 15

#include <vector>

using namespace std;

class PlayerLogin;
struct World;
struct Character;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);
class LoginPacket {
public:
	static void loginError(PlayerLogin* player, short errorid);
	static void loginProcess(PlayerLogin* player, char id);
	static void loginConnect(PlayerLogin* player, char* username, int size);
	static void processOk(PlayerLogin* player);
	static void showWorld(PlayerLogin* player, World world);
	static void worldEnd(PlayerLogin* player);
	static void showChannels(PlayerLogin* player);
	static void channelSelect(PlayerLogin* player);
	static void showCharacters(PlayerLogin* player, vector <Character> chars);
	static void showCharacter(PlayerLogin* player, Character charc);
	static void checkName(PlayerLogin* player, char is, char* name);
	static void deleteCharacter(PlayerLogin* player, int ID);
	static void connectIP(PlayerLogin* player, int charid);
	static void logBack(PlayerLogin* player);
};

#endif