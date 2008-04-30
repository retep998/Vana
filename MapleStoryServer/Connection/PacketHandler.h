#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "../Decoder/Decoder.h"
#include "../Decoder/MapleEncryption.h"
#include "Selector.h"
#include "AbstractPlayer.h"
//#include "Decoder.h"
//#include "GameManager.h"

class PacketHandler: public Selector::SelectHandler {
public:

	PacketHandler(int socket, AbstractPlayer* player);
	void handle (Selector* selector, int socket);
	void sendPacket(unsigned char* buf, int len);

private:

	unsigned char* buffer;
	int bytesInBuffer;
	AbstractPlayer* player;
	Decoder* decoder;
	int socket;
	//PlayerData pd;
	/*void HandleRequest(unsigned char* packet, int size, PlayerData &pd){
		short header = packet[0] + packet[1]*0x100;
		switch(header){
			case 0x01: GameManager::Login((const char*)packet+2, size-2, pd); break;
			case 0x04: GameManager::ChannelSelect(packet+2, pd); break;
			case 0x05: GameManager::SelectWorld(packet+2, pd); break;
			case 0x07: GameManager::SetGender(packet+2 ,pd); break;
			case 0x08: GameManager::HandleLogin(pd, packet+2); break;
			case 0x09: GameManager::RegisterPIN(packet+2, pd); break;
			case 0x0a: GameManager::ShowWorld(pd); break;
			case 0x0b: GameManager::ShowWorld(pd); break;
			case 0x0f: GameManager::ConnectGame(packet+2, pd); break;
			case 0x11: GameManager::CheckCharacterName(packet+2,pd); break;
			case 0x15: GameManager::CreateCharacter(packet+2,pd); break;
			case 0x16: GameManager::DeleteCharacter(packet+2,pd); break;
			case 0x1b: GameManager::LoginBack(pd); break;
		}
	}*/
};

#endif