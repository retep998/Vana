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
#include "Randomizer.h"
#include "sha1.h"

void Login::loginUser(PlayerLogin* player, unsigned char* packet){
	int usersize = BufferUtilities::getShort(packet);
	int passsize = BufferUtilities::getShort(packet+usersize+2);
	if(usersize > 15 || passsize > 15){
		return;
	}
	char username[MAX_FIELD_SIZE], password[MAX_FIELD_SIZE];
	BufferUtilities::getString(packet+2, usersize, username);   
	BufferUtilities::getString(packet+4+usersize, passsize, password);   

	mysqlpp::Query query = db.query();
	query << "SELECT id, password, salt, online, pin, gender, ban_reason, ban_expire, (ban_expire > NOW()) as banned FROM users WHERE username = " << mysqlpp::quote << username << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	bool valid = true;
	if (res.empty()) {
		LoginPacket::loginError(player, 0x05); //Invalid username
		valid = false;
	}
	else if (res[0]["salt"].is_null()) {
		// We have an unsalted password here
		if (strcmp(password, res[0]["password"])) {
			LoginPacket::loginError(player, 0x04); //Invalid password
			valid = false;
		}
		// We have a valid password here, so lets hash the password
		char *salt = generateSalt(5);
		char *hashed_pass = hashPassword(password, salt);
		query << "UPDATE users SET password = " << mysqlpp::quote << hashed_pass << ", salt = " << mysqlpp::quote << salt << " WHERE id = " << mysqlpp::quote << res[0]["id"];
		query.exec();
	}
	else if (strcmp(hashPassword(password, res[0]["salt"].c_str()), res[0]["password"].c_str())) {
		LoginPacket::loginError(player, 0x04); //Invalid password
		valid = false;
	}
	else if (atoi(res[0]["online"]) != 0) {
		LoginPacket::loginError(player, 0x07); //Already logged in
		valid = false;
	}
	else if (atoi(res[0]["banned"]) == 1) {
		int time = TimeUtilities::tickToTick32(TimeUtilities::timeToTick((time_t) mysqlpp::DateTime(res[0]["ban_expire"])));
		LoginPacket::loginBan(player, (unsigned char) res[0]["ban_reason"], time);
		valid = false;
	}
	if(!valid) {
		int threshold = LoginServer::Instance()->getInvalidLoginThreshold();
		if (threshold != 0 && player->addInvalidLogin() >= threshold) {
			player->disconnect(); // Too many invalid logins
		}
	}
	else {
		printf("%s logged in.\n", username);
		player->setUserid(res[0]["id"]);
		if (LoginServer::Instance()->getPinEnabled()) {
			if (res[0]["pin"].is_null())
				player->setPin(-1);
			else
				player->setPin(res[0]["pin"]);
			int pin = player->getPin();
			if(pin == -1)
				player->setStatus(1); // New PIN
			else
				player->setStatus(2); // Ask for PIN
		}
		else
			player->setStatus(4);
		if (res[0]["gender"].is_null())
			player->setStatus(5);
		else
			player->setGender((unsigned char) res[0]["gender"]);
		LoginPacket::loginConnect(player, username, usersize);
	}
}

void Login::setGender(PlayerLogin* player, unsigned char* packet){
	if (player->getStatus() != 5) {
		//hacking
		return;
	}
	if (packet[0] == 1) {
		player->setStatus(0);
		mysqlpp::Query query = db.query();
		query << "UPDATE users SET gender = " << mysqlpp::quote << packet[1] << " WHERE id = " << mysqlpp::quote << player->getUserid();
		query.exec();
		if (LoginServer::Instance()->getPinEnabled())
			player->setStatus(1); // Set pin
		else
			player->setStatus(4);
		LoginPacket::genderDone(player, packet[1]);
	}
}

void Login::handleLogin(PlayerLogin* player, unsigned char* packet){
	int status = player->getStatus();
	if(status == 1)
		LoginPacket::loginProcess(player, 0x01);
	else if(status == 2){
		LoginPacket::loginProcess(player, 0x04);
		player->setStatus(3);
	}
	else if(status == 3)
		checkPin(player, packet);
	else if(status == 4) {
		LoginPacket::loginProcess(player, 0x00);
		// The player successfully logged in, so let set the login column
		player->setOnline(true);
	}
}
void Login::checkPin(PlayerLogin* player, unsigned char* packet){
	if (!LoginServer::Instance()->getPinEnabled()) {
		//hacking
		return;
	}
	if(packet[0] == 0x00){
		player->setStatus(2);
	}
	else if(packet[0] == 0x01){
		int pin = (packet[8]-'0')*1000 + (packet[9]-'0')*100 + (packet[10]-'0')*10 + (packet[11]-'0');
		int curpin = player->getPin();
		if(pin == curpin){
			player->setStatus(4);
			handleLogin(player, packet);
			printf("Pin: %d\n", pin);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
	else if(packet[0] == 0x02){
		int pin = (packet[8]-'0')*1000 + (packet[9]-'0')*100 + (packet[10]-'0')*10 + (packet[11]-'0');
		int curpin = player->getPin();
		if(pin == curpin){
			player->setStatus(1);
			handleLogin(player, packet);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
}

void Login::registerPIN(PlayerLogin* player, unsigned char* packet){
	if (!LoginServer::Instance()->getPinEnabled() || player->getStatus() != 1) {
		//hacking
		return;
	}
	if(packet[0] == 0x00){
		if(player->getPin() != -1){
			player->setStatus(2);
		}
		return;
	}
	int pin = (packet[3]-'0')*1000 + (packet[4]-'0')*100 + (packet[5]-'0')*10 + (packet[6]-'0');
	player->setStatus(0);
	mysqlpp::Query query = db.query();
	query << "UPDATE users SET pin = " << mysqlpp::quote << pin << " WHERE id = " << mysqlpp::quote << player->getUserid();
	query.exec();
	LoginPacket::pinAssigned(player);
}

void Login::loginBack(PlayerLogin* player){
	LoginPacket::logBack(player);
}

char * Login::hashPassword(const char *password, const char *salt) {
	SHA1 sha;
	unsigned digest[5];
	char salted[50];
	sprintf_s(salted, "%s%s", salt, password);
	sha.Reset();
	sha.Input(salted, strlen(salted));
	sha.Result(digest);
	char *passhash = new char[45];
	sprintf_s(passhash,45,"%08X%08X%08X%08X%08X", digest[0], digest[1], digest[2], digest[3], digest[4]);

	return passhash;
}

char * Login::generateSalt(size_t length) {
	char *salt = new char[length+1];
	for (size_t i = 0; i < length; i++) {
		salt[i] = 33 + (Randomizer::Instance()->randInt(93));
	}
	salt[length] = 0;
	return salt;
}