#include "Worlds.h"
#include "LoginPacket.h"
#include <string.h>
#include "PlayerLogin.h"
#include "Characters.h"

void Worlds::showWorld(PlayerLogin* player){
	if(player->getStatus() != 4){
		// hacking
		return;
	}
	World world;
	strcpy_s(world.name, 15, "Scania");
	world.channels = 1;
	world.id = 0;
	LoginPacket::showWorld(player, world);
	LoginPacket::worldEnd(player);
}

void Worlds::selectWorld(PlayerLogin* player, unsigned char* packet){
	player->setServer(packet[0]);
	LoginPacket::showChannels(player);
}

void Worlds::channelSelect(PlayerLogin* player, unsigned char* packet){
	player->setChannel(packet[1]);
	LoginPacket::channelSelect(player);
	Characters::showCharacters(player);
}