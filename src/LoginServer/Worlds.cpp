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
#include "Channel.h"
#include "Characters.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "LoginServerAcceptConnection.h"
#include "LoginServerAcceptPacket.h"
#include "MapleSession.h"
#include "Player.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "PlayerStatus.h"
#include "World.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

Worlds * Worlds::singleton = nullptr;

void Worlds::showWorld(Player *player) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->isConnected()) {
			LoginPacket::showWorld(player, iter->second);
		}
	}
	LoginPacket::worldEnd(player);
}

void Worlds::addWorld(World *world) {
	worlds[world->getId()] = world;
}

void Worlds::selectWorld(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	uint8_t worldId = packet.get<uint8_t>();
	if (World *world = getWorld(worldId)) {
		player->setWorld(worldId);
		int32_t maxLoad = world->getMaxPlayerLoad();
		int32_t minMaxLoad = (maxLoad / 100) * 90; // 90% is enough for the many users warning, I think
		int8_t message = LoginPacket::WorldMessages::Normal;
		if (world->getPlayerLoad() >= minMaxLoad && world->getPlayerLoad() < maxLoad) {
			message = LoginPacket::WorldMessages::HeavyLoad;
		}
		else if (world->getPlayerLoad() == maxLoad) {
			message = LoginPacket::WorldMessages::MaxLoad;
		}
		LoginPacket::showChannels(player, message);
	}
	else {
		// Hacking of some sort...
		return;
	}
}

void Worlds::channelSelect(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	packet.skipBytes(1);
	int8_t channel = packet.get<int8_t>();

	LoginPacket::channelSelect(player);
	if (Channel *chan = worlds[player->getWorld()]->getChannel(channel)) {
		player->setChannel(channel);
		Characters::showCharacters(player);
	}
	else {
		LoginPacket::channelOffline(player);
	}
}

int8_t Worlds::addWorldServer(LoginServerAcceptConnection *player) {
	World *world = nullptr;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (!iter->second->isConnected()) {
			player->setWorldId(iter->first);
			world = iter->second;
			iter->second->setConnected(true);
			iter->second->setConnection(player);
			break;
		}
	}

	if (world != nullptr) {
		LoginServerAcceptPacket::connect(player, world);

		LoginServer::Instance()->log(LogTypes::ServerConnect, "World " + boost::lexical_cast<string>(static_cast<int16_t>(world->getId())));

		return world->getId();
	}
	else {
		LoginServerAcceptPacket::noMoreWorld(player);
		std::cout << "Error: No more worlds to assign." << std::endl;
		player->getSession()->disconnect();
		return -1;
	}
}

int8_t Worlds::addChannelServer(LoginServerAcceptConnection *player) {
	int8_t worldid = -1;
	uint16_t port;
	AbstractServerAcceptConnection *worldPlayer;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->getChannelCount() < iter->second->getMaxChannels() && iter->second->isConnected()) {
			worldid = iter->second->getId();
			port = iter->second->getPort();
			worldPlayer = iter->second->getConnection();
			break;
		}
	}

	if (worldid != -1) {
		uint32_t worldIp = IpUtilities::matchIpSubnet(player->getIp(), worldPlayer->getExternalIp(), worldPlayer->getIp());
		LoginServerAcceptPacket::connectServer(player, worldid, worldIp, port, InterChannelServer);
	}
	else {
		LoginServerAcceptPacket::connectServer(player, worldid, 0, 0, InterChannelServer);
		std::cout << "Error: No more channels to assign." << std::endl;
	}
	player->getSession()->disconnect();
	return worldid;
}

int8_t Worlds::addCashServer(LoginServerAcceptConnection *player) {
	int8_t worldid = -1;
	uint16_t port;
	AbstractServerAcceptConnection *worldPlayer;
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (!iter->second->isCashServerConnected() && iter->second->isConnected()) {
			worldid = iter->second->getId();
			port = iter->second->getPort();
			worldPlayer = iter->second->getConnection();
			iter->second->setCashServerConnected(true);
			break;
		}
	}

	if (worldid != -1) {
		uint32_t worldIp = IpUtilities::matchIpSubnet(player->getIp(), worldPlayer->getExternalIp(), worldPlayer->getIp());
		LoginServerAcceptPacket::connectServer(player, worldid, worldIp, port, InterCashServer);
	}
	else {
		LoginServerAcceptPacket::connectServer(player, worldid, 0, 0, InterCashServer);
		std::cout << "Error: No more cash servers to assign." << std::endl;
	}
	player->getSession()->disconnect();
	return worldid;
}

void Worlds::toWorlds(PacketCreator &packet) {
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (iter->second->isConnected()) {
			iter->second->getConnection()->getSession()->send(packet);
		}
	}
}

void Worlds::runFunction(function<bool (World *)> func) {
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		if (func(iter->second)) {
			break;
		}
	}
}

namespace Functors {
	struct PlayerLoad {
		void operator()(Channel *channel) {
			world->setPlayerLoad(world->getPlayerLoad() + channel->getPopulation());
		}
		World *world;
	};
}

void Worlds::calculatePlayerLoad(World *world) {
	world->setPlayerLoad(0);
	Functors::PlayerLoad load = {world};
	world->runChannelFunction(load);
}

World * Worlds::getWorld(uint8_t id) {
	return worlds.find(id) == worlds.end() ? nullptr : worlds[id];
}

void Worlds::setEventMessages(const string &message) {
	for (map<uint8_t, World *>::iterator iter = worlds.begin(); iter != worlds.end(); iter++) {
		iter->second->setEventMessage(message);
	}
}