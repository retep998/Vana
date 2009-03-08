/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Characters.h"
#include "LoginPacket.h"
#include "LoginServerAcceptPlayer.h"
#include "LoginServerAcceptPacket.h"
#include "MapleSession.h"
#include "PlayerLogin.h"
#include "PacketReader.h"

map<uint8_t, World *> Worlds::worlds;

void Worlds::showWorld(PlayerLogin *player) {
	if (player->getStatus() != 4) {
		// hacking
		return;
	}

	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++)
		if (iter->second->connected == true)
			LoginPacket::showWorld(player, iter->second);
	LoginPacket::worldEnd(player);
}

void Worlds::selectWorld(PlayerLogin *player, PacketReader &packet) {
	if (player->getStatus() != 4) {
		// hacking
		return;
	}
	player->setWorld(packet.get<int8_t>());
	LoginPacket::showChannels(player);
}

void Worlds::channelSelect(PlayerLogin *player, PacketReader &packet) {
	if (player->getStatus() != 4) {
		// hacking
		return;
	}
	packet.skipBytes(1);
	player->setChannel(packet.get<int8_t>());
	LoginPacket::channelSelect(player);
	Characters::showCharacters(player);
}

int8_t Worlds::connectWorldServer(LoginServerAcceptPlayer *player) {
	World *world = 0;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->connected == 0) {
			player->setWorldId(iter->first);
			world = iter->second;
			iter->second->connected = true;
			iter->second->player = player;
			iter->second->ip = player->getIP();
			break;
		}
	}
	
	if (world != 0) {
		LoginServerAcceptPacket::connect(player, world);
		std::cout << "Assigned world " << (int32_t) world->id << " to World Server." << std::endl;
		return world->id;
	}
	else {
		LoginServerAcceptPacket::noMoreWorld(player);
		std::cout << "Error: No more worlds to assign." << std::endl;
		player->getSession()->disconnect();
		return -1;
	}
}

int8_t Worlds::connectChannelServer(LoginServerAcceptPlayer *player) {
	int8_t worldid = -1;
	int16_t port;
	uint32_t ip;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->channels.size() < (size_t) iter->second->maxChannels && iter->second->connected) {
			worldid = iter->second->id;
			port = iter->second->port;
			ip = iter->second->ip;
			break;
		}
	}
	LoginServerAcceptPacket::connectChannel(player, worldid, ip, port);
	if (worldid != -1) {
		std::cout << "Assigning channel server to world server " << (int32_t) worldid << "." << std::endl;
	}
	else {
		std::cout << "Error: No more channels to assign." << std::endl;
	}
	player->getSession()->disconnect();
	return worldid;
}
