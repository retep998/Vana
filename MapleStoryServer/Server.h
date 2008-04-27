#ifndef SERVER_H
#define SERVER_H

class Player;

namespace Server {
	extern private char msg[100];
	extern private int on;
	void showScrollingHeader(Player* player);
	void changeScrollingHeader(char* msg);
	void enableScrollingHeader();
	void disableScrollingHeader();
	void initialize();
};

#endif