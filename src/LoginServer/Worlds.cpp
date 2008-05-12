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
#include "Worlds.h"
#include "LoginPacket.h"
#include "WorldServerPacket.h"
#include <string.h>
#include "PlayerLogin.h"
#include "LoginServerAcceptPlayer.h"
#include "Characters.h"

hash_map <int, World> Worlds::worlds;

void Worlds::showWorld(PlayerLogin* player){
	if(player->getStatus() != 4){
		// hacking
		return;
	}

	for (hash_map <int, World>::iterator iter = worlds.begin(); iter != worlds.end(); iter++)
		if (iter->second.connected == true)
			LoginPacket::showWorld(player, &iter->second);
	LoginPacket::worldEnd(player);
}

void Worlds::selectWorld(PlayerLogin* player, unsigned char* packet){
	if(player->getStatus() != 4){
		// hacking
		return;
	}
	player->setServer(packet[0]);
	LoginPacket::showChannels(player);
}

void Worlds::channelSelect(PlayerLogin* player, unsigned char* packet){
	if(player->getStatus() != 4){
		// hacking
		return;
	}
	player->setChannel(packet[1]);
	LoginPacket::channelSelect(player);
	Characters::showCharacters(player);
}

char Worlds::connectWorldServer(LoginServerAcceptPlayer *player) {
	char worldid = -1;
	for (hash_map <int, World>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second.connected == 0) {
			worldid = iter->second.id;
			iter->second.connected = true;
			break;
		}
	}
	WorldServerPacket::connect(player, worldid);
	if (worldid != -1) {
		std::cout << "Assigned world " << (int) worldid << " to World Server." << std::endl;
	}
	else {
		std::cout << "Error: No more world to assign." << std::endl;
		player->disconnect();
	}
	return worldid;
}
