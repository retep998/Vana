#ifndef WORLDS_H
#define WORLDS_H

class PlayerLogin;

struct World {
	char name[15];
	int channels;
	char id;
};

class Worlds {
public:
	static void channelSelect(PlayerLogin* player, unsigned char* packet);
	static void selectWorld(PlayerLogin* player, unsigned char* packet);
	static void showWorld(PlayerLogin* player);
};

#endif