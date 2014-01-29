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
#include "LoginServerAcceptHandler.hpp"
#include "Channel.hpp"
#include "LoginServer.hpp"
#include "LoginServerAcceptConnection.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "StringUtilities.hpp"
#include "World.hpp"
#include "Worlds.hpp"
#include <iostream>

auto LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t channel = packet.get<int32_t>();
	Channel *chan = new Channel();
	const Ip &ip = packet.getClass<Ip>();

	chan->setExternalIpInformation(ip, packet.getClassVector<ExternalIp>());
	chan->setPort(packet.get<port_t>());
	Worlds::getInstance().getWorld(connection->getWorldId())->addChannel(channel, chan);
	LoginServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(connection->getWorldId()) << "; Channel " << channel; });
}

auto LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t channel = packet.get<int32_t>();
	int32_t population = packet.get<int32_t>();

	World *world = Worlds::getInstance().getWorld(connection->getWorldId());
	world->getChannel(channel)->setPopulation(population);
	Worlds::getInstance().calculatePlayerLoad(world);
}

auto LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t channel = packet.get<int32_t>();

	Worlds::getInstance().getWorld(connection->getWorldId())->removeChannel(channel);
	LoginServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(connection->getWorldId()) << "; Channel " << channel; });
}

auto LoginServerAcceptHandler::sendPacketToWorlds(LoginServerAcceptConnection *connection, PacketReader &packet) -> void {
	PacketCreator pack;
	pack.addBuffer(packet);
	Worlds::getInstance().toWorlds(pack);
}