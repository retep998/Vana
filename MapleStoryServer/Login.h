#ifndef LOGIN_H
#define LOGIN_H

class PlayerLogin;

class Login {
private:
	static int checkLogin(char *username, char *password);
	static void checkPin(PlayerLogin* player, unsigned char* packet);
public:
	static void loginUser(PlayerLogin* player, unsigned char* packet);
	static void setGender(PlayerLogin* player, unsigned char* packet){}
	static void handleLogin(PlayerLogin* player, unsigned char* packet);
	static void registerPIN(PlayerLogin* player, unsigned char* packet);
	static void loginBack(PlayerLogin* player);
};

#endif