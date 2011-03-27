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
#include "LoginServerAcceptHandler.h"
#include "Channel.h"
#include "IpUtilities.h"
#include "LoginServer.h"
#include "LoginServerAcceptConnection.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "World.h"
#include "Worlds.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::lexical_cast;

void LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *connection, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	Channel *chan = new Channel();
	chan->setIp(packet.get<ip_t>());
	IpUtilities::extractExternalIp(packet, chan->getExternalIps());
	chan->setPort(packet.get<port_t>());
	Worlds::Instance()->getWorld(connection->getWorldId())->addChannel(channel, chan);
	LoginServer::Instance()->log(LogTypes::ServerConnect, "World " + lexical_cast<string>(static_cast<int16_t>(connection->getWorldId())) + "; Channel " + lexical_cast<string>(channel));
}

void LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *connection, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	int32_t population = packet.get<int32_t>();

	World *world = Worlds::Instance()->getWorld(connection->getWorldId());
	world->getChannel(channel)->setPopulation(population);
	Worlds::Instance()->calculatePlayerLoad(world);
}

void LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *connection, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();

	Worlds::Instance()->getWorld(connection->getWorldId())->removeChannel(channel);
	LoginServer::Instance()->log(LogTypes::ServerDisconnect, "World " + lexical_cast<string>(static_cast<int16_t>(connection->getWorldId())) + "; Channel " + lexical_cast<string>(channel));
}

void LoginServerAcceptHandler::toWorlds(LoginServerAcceptConnection *connection, PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Worlds::Instance()->toWorlds(pack);
}