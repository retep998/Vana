#ifndef LOGIN_H
#define LOGIN_H

class PlayerLogin;

namespace Login {
	void loginUser(PlayerLogin* player, unsigned char* packet);
	void setGender(PlayerLogin* player, unsigned char* packet);
	void handleLogin(PlayerLogin* player, unsigned char* packet);
	void registerPIN(PlayerLogin* player, unsigned char* packet);
	void loginBack(PlayerLogin* player);
	extern private int checkLogin(char *username, char *password);
	extern private void checkPin(PlayerLogin* player, unsigned char* packet);
};

#endif