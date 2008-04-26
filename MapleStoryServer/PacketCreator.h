#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#define MAX_LEN 10000
#include <Winsock2.h>
#include "PlayerLogin.h"
#include "Player.h"
#include <Vector>
using namespace std;

class Packet {
public:
	Packet(){
		this->pos=2;
	}
	void addHeader(short headerid);
	void addInt(int intg);
	void addInt64(__int64 int64);
	void addShort(short shrt);
	void addString(char* str, int slen);
	void addByte(unsigned char byte);
	void addBytes(char* hex);
	void addBytesHex(unsigned char* bytes, int len);
	void packetSend(Player* player);	
	void packetSendLogin(PlayerLogin* player);	
	void sendTo(Player* player, vector <Player*> players, bool is);
private:
	int pos;
	SOCKET socketid;
	unsigned char packet[MAX_LEN];
};

#endif