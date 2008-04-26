#include "PlayerLogin.h"
#include "Characters.h"
#include "Login.h"
#include "Worlds.h"

void PlayerLogin::handleRequest(unsigned char* buf, int len){
	short header = buf[0] + buf[1]*0x100;
	switch(header){
		case 0x1b: Login::loginUser(this, buf+2); break;
		case 0x19: Worlds::channelSelect(this, buf+2); break;
		case 0x13: Worlds::selectWorld(this, buf+2); break;
		case 0x03: Login::handleLogin(this, buf+2); break;
		case 0x18: Worlds::showWorld(this); break;
		case 0x02: Worlds::showWorld(this); break;
		case 0x16: Characters::connectGame(this, buf+2); break;
		case 0x09: Characters::checkCharacterName(this, buf+2); break;
		case 0x0e: Characters::createCharacter(this, buf+2); break;
		case 0x0f: Characters::deleteCharacter(this, buf+2); break;
		case 0x1A: Login::loginBack(this); break;
		//case 0x07: Login::setGender(this, buf+2); break;
		//case 0x09: Login::registerPIN(this, buf+2); break;
	}
}