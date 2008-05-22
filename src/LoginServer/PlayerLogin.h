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
#ifndef PLAYERLOGIN_H
#define PLAYERLOGIN_H

#include "AbstractPlayer.h"
#include "Connection/PacketHandler.h"

class PlayerLogin:public AbstractPlayer {
public:
	PlayerLogin () {
		status=0;
		invalid_logins = 0;
	}

	~PlayerLogin();

	void realHandleRequest(unsigned char* buf, int len);
	void setUserid(int id){
		this->userid=id;
	}
	int getUserid(){
		return this->userid;
	}
	void setStatus(int status){
		this->status=status;
	}
	int getStatus(){
		return this->status;
	}
	void setPin(int pin){
		this->pin=pin;
	}
	int getPin(){
		return this->pin;
	}
	void setGender(char gender){
		this->gender=gender;
	}
	char getGender(){
		return this->gender;
	}
	void setWorld(char world){
		this->world = world;
	}
	char getWorld(){
		return this->world;
	}
	void setChannel(int channel){
		this->channel=channel;
	}
	int getChannel(){
		return this->channel;
	}
	int addInvalidLogin() { return ++invalid_logins; }
	void setOnline(bool online);
private:
	int status;
	int userid;
	int pin;
	bool checked_pin;
	char gender;
	char world;
	int channel;
	int invalid_logins;
};

class PlayerLoginFactory:public AbstractPlayerFactory {
public:
	AbstractPlayer* createPlayer() {
		return new PlayerLogin();
	}
};

#endif