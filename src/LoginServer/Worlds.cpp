/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "IpUtilities.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "LoginServerAcceptConnection.h"
#include "LoginServerAcceptPacket.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "Session.h"
#include "StringUtilities.h"
#include "World.h"
#include <iostream>

Worlds * Worlds::singleton = nullptr;

void Worlds::showWorld(Player *player) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		if (iter->second->isConnected()) {
			LoginPacket::showWorld(player, iter->second);
		}
	}
	LoginPacket::worldEnd(player);
}

void Worlds::addWorld(World *world) {
	m_worlds[world->getId()] = world;
}

void Worlds::selectWorld(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	int8_t worldId = packet.get<int8_t>();
	if (World *world = getWorld(worldId)) {
		player->setWorldId(worldId);
		int32_t load = world->getPlayerLoad();
		int32_t maxLoad = world->getMaxPlayerLoad();
		int32_t minMaxLoad = (maxLoad / 100) * 90;
		int8_t message = LoginPacket::WorldMessages::Normal;

		if (load >= minMaxLoad && load < maxLoad) {
			message = LoginPacket::WorldMessages::HeavyLoad;
		}
		else if (load == maxLoad) {
			message = LoginPacket::WorldMessages::MaxLoad;
		}
		LoginPacket::showChannels(player, message);
	}
	else {
		// Hacking
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
	World *world = m_worlds[player->getWorldId()];

	if (world == nullptr) {
		// Hacking, lag, or client that hasn't been updated (e.g. in the middle of logging in)
		return;
	}

	if (Channel *chan = world->getChannel(channel)) {
		player->setChannel(channel);
		Characters::showCharacters(player);
	}
	else {
		LoginPacket::channelOffline(player);
	}
}

int8_t Worlds::addWorldServer(LoginServerAcceptConnection *connection) {
	World *world = nullptr;
	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		if (!iter->second->isConnected()) {
			world = iter->second;
			break;
		}
	}

	int8_t worldId = -1;
	if (world != nullptr) {
		worldId = world->getId();
		connection->setWorldId(worldId);
		world->setConnected(true);
		world->setConnection(connection);

		LoginServerAcceptPacket::connect(connection, world);

		LoginServer::Instance()->log(LogTypes::ServerConnect, "World " + StringUtilities::lexical_cast<string>(worldId));
	}
	else {
		LoginServerAcceptPacket::noMoreWorld(connection);
		std::cerr << "Error: No more worlds to assign." << std::endl;
		connection->getSession()->disconnect();
	}
	return worldId;
}

int8_t Worlds::addChannelServer(LoginServerAcceptConnection *connection) {
	World *validWorld = nullptr;
	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		World *world = iter->second;
		if (world->getChannelCount() < world->getMaxChannels() && world->isConnected()) {
			validWorld = world;
			break;
		}
	}

	int8_t worldId = -1;
	if (validWorld != nullptr) {
		worldId = validWorld->getId();
		LoginServerAcceptConnection *wConnection = validWorld->getConnection();

		ip_t worldIp = IpUtilities::matchIpSubnet(connection->getIp(), wConnection->getExternalIp(), wConnection->getIp());
		LoginServerAcceptPacket::connectChannel(connection, worldId, worldIp, validWorld->getPort());
	}
	else {
		LoginServerAcceptPacket::connectChannel(connection, worldId, 0, 0);
		std::cerr << "Error: No more channels to assign." << std::endl;
	}
	connection->getSession()->disconnect();
	return worldId;
}

void Worlds::toWorlds(PacketCreator &packet) {
	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		if (iter->second->isConnected()) {
			iter->second->getConnection()->getSession()->send(packet);
		}
	}
}

void Worlds::runFunction(function<bool (World *)> func) {
	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		if (func(iter->second)) {
			break;
		}
	}
}

void Worlds::calculatePlayerLoad(World *world) {
	world->setPlayerLoad(0);
	world->runChannelFunction([&world](Channel *channel) {
		world->setPlayerLoad(world->getPlayerLoad() + channel->getPopulation());
	});
}

World * Worlds::getWorld(int8_t id) {
	return m_worlds.find(id) == m_worlds.end() ? nullptr : m_worlds[id];
}

void Worlds::setEventMessages(const string &message) {
	for (map<int8_t, World *>::iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter) {
		iter->second->setEventMessage(message);
	}
}