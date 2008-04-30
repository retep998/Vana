#ifndef PLAYERLOGIN_H
#define PLAYERLOGIN_H

#include "Connection/AbstractPlayer.h"
#include "Connection/PacketHandler.h"

class PlayerLogin:public AbstractPlayer {
public:
	PlayerLogin () {
		status=0;
		idnum=0;
	}

	~PlayerLogin() {

	}
	void handleRequest(unsigned char* buf, int len);
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
	void setServer(int server){
		this->server=server;
	}
	int getServer(){
		return this->server;
	}
	void setChannel(int channel){
		this->channel=channel;
	}
	int getChannel(){
		return this->channel;
	}
	void setIDs(int id[3], int idnum){
		for(int i=0; i<idnum; i++){
			IDs[i] = id[i];
		}
		this->idnum = idnum;
	}
	int getIDs(int id[3]){
		for(int i=0; i<idnum; i++){
			id[i] = IDs[i];
		}
		return idnum;
	}
	void sendPacket(unsigned char* buf, int len){ packetHandler->sendPacket(buf,len); }
private:
	int status;
	int userid;
	int pin;
	char gender;
	int server;
	int channel;
	int IDs[3];
	int idnum;
	
};

class PlayerLoginFactory:public AbstractPlayerFactory {
public:
	AbstractPlayer* createPlayer() {
		return new PlayerLogin();
	}
};

#endif