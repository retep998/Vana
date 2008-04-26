#ifndef SERVER_H
#define SERVER_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

class Player;

class Server {
private: 
	static char msg[100];
	static int on;
public:
	static void showScrollingHeader(Player* player);
	static void changeScrollingHeader(char* msg);
	static void enableScrollingHeader();
	static void disableScrollingHeader();
	static void initialize();
	static __int64 getServerTime();
};

#endif