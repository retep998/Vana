#ifndef WORLDS_H
#define WORLDS_H

class PlayerLogin;

struct World {
	char name[15];
	int channels;
	char id;
};

namespace Worlds {
	void channelSelect(PlayerLogin* player, unsigned char* packet);
	void selectWorld(PlayerLogin* player, unsigned char* packet);
	void showWorld(PlayerLogin* player);
};

#endif