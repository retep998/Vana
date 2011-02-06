/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "IpUtilities.h"
#include "LoginPacket.h"
#include "LoginServerAcceptConnection.h"
#include "LoginServerAcceptPacket.h"
#include "MapleSession.h"
#include "PlayerLogin.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "PlayerStatus.h"
#include <iostream>

map<uint8_t, World *> Worlds::worlds;

void Worlds::showWorld(PlayerLogin *player) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// hacking
		return;
	}

	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++)
		if (iter->second->connected == true)
			LoginPacket::showWorld(player, iter->second);
	LoginPacket::worldEnd(player);
}

void Worlds::selectWorld(PlayerLogin *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// hacking
		return;
	}
	uint8_t worldId = packet.get<uint8_t>();
	if (World *world = getWorld(worldId)) {
		player->setWorld(worldId);
		int32_t minMaxLoad = (world->maxPlayerLoad / 100) * 90; // 90% is enough for the many users warning, i think.
		int8_t message = 0x00;
		if (world->currentPlayerLoad >= minMaxLoad && world->currentPlayerLoad < world->maxPlayerLoad)
			message = 0x01;
		else if (world->currentPlayerLoad == world->maxPlayerLoad)
			message = 0x02;

		LoginPacket::showChannels(player, message);
	}
	else {
		// hacking of some sort...
		return;
	}
}

void Worlds::channelSelect(PlayerLogin *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// hacking
		return;
	}
	packet.skipBytes(1);
	int8_t channel = packet.get<int8_t>();
	
	LoginPacket::channelSelect(player);
	World *world = worlds[player->getWorld()];
	if (world->channels.find(channel) != world->channels.end()) {
		player->setChannel(channel);
		Characters::showCharacters(player);
	}
	else {
		LoginPacket::channelOffline(player);
	}
}

int8_t Worlds::connectWorldServer(LoginServerAcceptConnection *player) {
	World *world = 0;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->connected == 0) {
			player->setWorldId(iter->first);
			world = iter->second;
			iter->second->connected = true;
			iter->second->player = player;
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

int8_t Worlds::connectChannelServer(LoginServerAcceptConnection *player) {
	int8_t worldid = -1;
	int16_t port;
	AbstractServerAcceptConnection *worldPlayer;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->channels.size() < (size_t) iter->second->maxChannels && iter->second->connected) {
			worldid = iter->second->id;
			port = iter->second->port;
			worldPlayer = iter->second->player;
			break;
		}
	}

	uint32_t worldIp = IpUtilities::matchIpSubnet(player->getIp(), worldPlayer->getExternalIp(), worldPlayer->getIp());
	LoginServerAcceptPacket::connectChannel(player, worldid, worldIp, port);
	if (worldid != -1) {
		std::cout << "Assigning channel server to world server " << (int32_t) worldid << "." << std::endl;
	}
	else {
		std::cout << "Error: No more channels to assign." << std::endl;
	}
	player->getSession()->disconnect();
	return worldid;
}

void Worlds::toWorlds(PacketCreator &packet) {
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++)
		if (iter->second->connected == true)
			iter->second->player->getSession()->send(packet);
}

void Worlds::calculatePlayerLoad(World *world) {
	world->currentPlayerLoad = 0;
	for (size_t i = 0; i < world->maxChannels; i++) {
		if (world->channels.find(i) != world->channels.end())
			world->currentPlayerLoad += world->channels[i]->population;
	}
}

World * Worlds::getWorld(uint8_t id) {
	return worlds.find(id) == worlds.end() ? 0 : worlds[id];
}
