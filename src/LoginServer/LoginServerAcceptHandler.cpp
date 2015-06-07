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
#include "Channel.hpp"
#include "LoginServer.hpp"
#include "LoginServerAcceptConnection.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "StringUtilities.hpp"
#include "World.hpp"
#include "Worlds.hpp"
#include <iostream>

auto LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	Channel *chan = new Channel();
	const Ip &ip = reader.get<Ip>();

	chan->setExternalIpInformation(ip, reader.get<vector_t<ExternalIp>>());
	chan->setPort(reader.get<port_t>());
	LoginServer::getInstance().getWorlds().getWorld(connection->getWorldId())->addChannel(channel, chan);
	LoginServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(connection->getWorldId()) << "; Channel " << static_cast<int32_t>(channel); });
}

auto LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	int32_t population = reader.get<int32_t>();

	World *world = LoginServer::getInstance().getWorlds().getWorld(connection->getWorldId());
	world->getChannel(channel)->setPopulation(population);
	LoginServer::getInstance().getWorlds().calculatePlayerLoad(world);
}

auto LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *connection, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();

	LoginServer::getInstance().getWorlds().getWorld(connection->getWorldId())->removeChannel(channel);
	LoginServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(connection->getWorldId()) << "; Channel " << static_cast<int32_t>(channel); });
}
