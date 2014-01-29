/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Worlds.hpp"
#include "Channel.hpp"
#include "Characters.hpp"
#include "LoginPacket.hpp"
#include "LoginServer.hpp"
#include "LoginServerAcceptConnection.hpp"
#include "LoginServerAcceptPacket.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerStatus.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "World.hpp"
#include <iostream>

auto Worlds::showWorld(Player *player) -> void {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	for (const auto &kvp : m_worlds) {
		if (kvp.second->isConnected()) {
			LoginPacket::showWorld(player, kvp.second);
		}
	}
	LoginPacket::worldEnd(player);
}

auto Worlds::addWorld(World *world) -> void {
	m_worlds[world->getId()] = world;
}

auto Worlds::selectWorld(Player *player, PacketReader &packet) -> void {
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

auto Worlds::channelSelect(Player *player, PacketReader &packet) -> void {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	packet.skipBytes(1);
	int8_t channelId = packet.get<int8_t>();

	LoginPacket::channelSelect(player);
	World *world = m_worlds[player->getWorldId()];

	if (world == nullptr) {
		// Hacking, lag, or client that hasn't been updated (e.g. in the middle of logging in)
		return;
	}

	if (Channel *channel = world->getChannel(channelId)) {
		player->setChannel(channelId);
		Characters::showCharacters(player);
	}
	else {
		LoginPacket::channelOffline(player);
	}
}

auto Worlds::addWorldServer(LoginServerAcceptConnection *connection) -> int8_t {
	World *world = nullptr;
	for (const auto &kvp : m_worlds) {
		if (!kvp.second->isConnected()) {
			world = kvp.second;
			break;
		}
	}

	int8_t worldId = -1;
	if (world != nullptr) {
		worldId = world->getId();
		connection->setWorldId(worldId);
		world->setConnected(true);
		world->setConnection(connection);

		LoginServerAcceptPacket::connect(world);

		LoginServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(worldId); });
	}
	else {
		LoginServerAcceptPacket::noMoreWorld(connection);
		std::cerr << "ERROR: No more worlds to assign." << std::endl;
		connection->getSession()->disconnect();
	}
	return worldId;
}

auto Worlds::addChannelServer(LoginServerAcceptConnection *connection) -> int8_t {
	World *validWorld = nullptr;
	for (const auto &kvp : m_worlds) {
		World *world = kvp.second;
		if (world->getChannelCount() < world->getMaxChannels() && world->isConnected()) {
			validWorld = world;
			break;
		}
	}

	int8_t worldId = -1;
	if (validWorld != nullptr) {
		worldId = validWorld->getId();
		Ip worldIp = validWorld->matchSubnet(connection->getIp());
		LoginServerAcceptPacket::connectChannel(connection, worldId, worldIp, validWorld->getPort());
	}
	else {
		LoginServerAcceptPacket::connectChannel(connection, worldId, Ip(0), 0);
		std::cerr << "ERROR: No more channels to assign." << std::endl;
	}
	connection->getSession()->disconnect();
	return worldId;
}

auto Worlds::toWorlds(PacketCreator &packet) -> void {
	for (const auto &kvp : m_worlds) {
		if (kvp.second->isConnected()) {
			kvp.second->send(packet);
		}
	}
}

auto Worlds::runFunction(function_t<bool (World *)> func) -> void {
	for (const auto &kvp : m_worlds) {
		if (func(kvp.second)) {
			break;
		}
	}
}

auto Worlds::calculatePlayerLoad(World *world) -> void {
	world->setPlayerLoad(0);
	world->runChannelFunction([&world](Channel *channel) {
		world->setPlayerLoad(world->getPlayerLoad() + channel->getPopulation());
	});
}

auto Worlds::getWorld(int8_t id) -> World * {
	auto kvp = m_worlds.find(id);
	return kvp != std::end(m_worlds) ? kvp->second : nullptr;
}

auto Worlds::setEventMessages(const string_t &message) -> void {
	for (const auto &kvp : m_worlds) {
		kvp.second->setEventMessage(message);
	}
}