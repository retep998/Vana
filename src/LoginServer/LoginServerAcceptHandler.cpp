/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "LoginServerAcceptHandler.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/StringUtilities.hpp"
#include "LoginServer/Channel.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/LoginServerAcceptConnection.hpp"
#include "LoginServer/World.hpp"
#include "LoginServer/Worlds.hpp"
#include <iostream>

namespace Vana {
namespace LoginServer {

auto LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	Channel *chan = new Channel();
	const Ip &ip = reader.get<Ip>();
	optional_t<world_id_t> worldId = connection->getWorldId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}

	chan->setExternalIpInformation(ip, reader.get<vector_t<ExternalIp>>());
	chan->setPort(reader.get<port_t>());
	LoginServer::getInstance().getWorlds().getWorld(worldId.get())->addChannel(channel, chan);
	LoginServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) {
		log << "World " << static_cast<int32_t>(worldId.get()) << "; Channel " << static_cast<int32_t>(channel);
	});
}

auto LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	int32_t population = reader.get<int32_t>();
	optional_t<world_id_t> worldId = connection->getWorldId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}

	auto &worlds = LoginServer::getInstance().getWorlds();
	World *world = worlds.getWorld(worldId.get());
	world->getChannel(channel)->setPopulation(population);
	worlds.calculatePlayerLoad(world);
}

auto LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();

	optional_t<world_id_t> worldId = connection->getWorldId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}

	LoginServer::getInstance().getWorlds().getWorld(worldId.get())->removeChannel(channel);
	LoginServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) {
		log << "World " << static_cast<int32_t>(worldId.get()) << "; Channel " << static_cast<int32_t>(channel);
	});
}

}
}