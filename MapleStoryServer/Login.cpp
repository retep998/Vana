/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Login.h"
#include "LoginPacket.h"
#include "MySQLM.h"
#include "BufferUtilities.h"
#include <stdio.h>
#include "PlayerLogin.h"

void Login::loginUser(PlayerLogin* player, unsigned char* packet){
	int usersize = getShort(packet);
	int passsize = getShort(packet+usersize+2);
	if(usersize > 15 || passsize > 15){
		return;
	}
	char username[MAX_FIELD_SIZE], password[MAX_FIELD_SIZE];
	getString(packet+2, usersize, username);   
	getString(packet+4+usersize, passsize, password);   
	int s = checkLogin(username, password);
	if(s == 1){
		printf("%s logged in.\n", username);
		player->setUserid(MySQL::getUserID(username));
		player->setPin(MySQL::getInt("users", player->getUserid(), "pin"));
		int pin = player->getPin();
		if(pin == -1)
			player->setStatus(1); // New PIN
		else
			player->setStatus(2); // Ask for PIN
		player->setGender(MySQL::getInt("users", player->getUserid(), "gender"));
		LoginPacket::loginConnect(player, username, usersize);
	}
	else if(s == 0)
		LoginPacket::loginError(player, 0x04);
	else if(s == -1)
		LoginPacket::loginError(player, 0x05);
}

void Login::setGender(PlayerLogin* player, unsigned char* packet){
	//TODO
}

int Login::checkLogin(char *username, char *password){
	char rpassword[13];
	if(!MySQL::isString("users","username", username)){
		return -1;
	}
	MySQL::getString("users", "username",username,"password", rpassword);
	if(strcmp(rpassword, password) == 0)
		return 1;
	return 0;
}

void Login::handleLogin(PlayerLogin* player, unsigned char* packet){
	int status = player->getStatus();
	if(status == 1)
		LoginPacket::loginProcess(player, 0x01);
	else if(status == 2){
		//LoginPacket::loginProcess(player, 0x04);
		//player->setStatus(3);
		LoginPacket::loginProcess(player, 0x00);
		player->setStatus(4);
	}
	else if(status == 3)
		checkPin(player, packet);
	else if(status == 4)
		LoginPacket::loginProcess(player, 0x00);
}
void Login::checkPin(PlayerLogin* player, unsigned char* packet){
	if(packet[0] == 0x00){
		player->setStatus(2);
	}
	else if(packet[0] == 0x01){
		int pin = (packet[8]-'0')*1000 + (packet[9]-'0')*100 + (packet[10]-'0')*10 + (packet[11]-'0');
		int curpin = player->getPin();
		if(pin == curpin){
			LoginPacket::loginProcess(player, 0x00);
			player->setStatus(4);
			printf("Pin: %d\n", pin);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
	else if(packet[0] == 0x02){
		int pin = (packet[8]-'0')*1000 + (packet[9]-'0')*100 + (packet[10]-'0')*10 + (packet[11]-'0');
		int curpin = player->getPin();
		if(pin == curpin){
			LoginPacket::loginProcess(player, 0x01);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
}

void Login::registerPIN(PlayerLogin* player, unsigned char* packet){
	if(packet[0] == 0x00){
		if(player->getPin() != -1){
			player->setStatus(2);
		}
		return;
	}
	int pin = (packet[3]-'0')*1000 + (packet[4]-'0')*100 + (packet[5]-'0')*10 + (packet[6]-'0');
	player->setStatus(0);
	MySQL::setInt("users", "pin", player->getUserid(), pin);
	LoginPacket::processOk(player);
}

void Login::loginBack(PlayerLogin* player){
	LoginPacket::logBack(player);
}