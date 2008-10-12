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
#include "Types.h"

class ReadPacket;

class PlayerLogin : public AbstractPlayer {
public:
	PlayerLogin() : status(0), invalid_logins(0) { }

	~PlayerLogin();

	void realHandleRequest(ReadPacket &packet);

	void setGender(int8_t gender) { this->gender=gender; }
	void setWorld(int8_t world) { this->world = world; }
	void setChannel(uint16_t channel) {	this->channel = channel; }
	void setUserid(int32_t id) { this->userid = id; }
	void setStatus(int32_t status) { this->status = status; }
	void setPin(int32_t pin) { this->pin = pin; }

	int8_t getGender() const { return this->gender; }
	int8_t getWorld() const { return this->world; }
	uint16_t getChannel() const { return this->channel; }
	int32_t getUserid() const { return this->userid; }
	int32_t getStatus() const { return this->status; }
	int32_t getPin() const { return this->pin; }
	int32_t addInvalidLogin() {	return ++invalid_logins; }

	void setOnline(bool online);
private:
	int8_t gender;
	int8_t world;
	uint16_t channel;
	int32_t status;
	int32_t userid;
	int32_t pin;
	int32_t invalid_logins;
	bool checked_pin;
};

class PlayerLoginFactory : public AbstractPlayerFactory {
public:
	AbstractPlayer * createPlayer() {
		return new PlayerLogin();
	}
};

#endif
