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

void LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	Channel *chan = new Channel();
	chan->setIp(packet.get<uint32_t>());
	IpUtilities::extractExternalIp(packet, chan->getExternalIps());
	chan->setPort(packet.get<uint16_t>());
	Worlds::Instance()->getWorld(player->getWorldId())->addChannel(channel, chan);

	LoginServer::Instance()->log(LogTypes::ServerConnect, "World " + lexical_cast<string>(static_cast<int16_t>(player->getWorldId())) + "; Channel " + lexical_cast<string>(channel));
}

void LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	int32_t population = packet.get<int32_t>();

	World *world = Worlds::Instance()->getWorld(player->getWorldId());
	world->getChannel(channel)->setPopulation(population);
	Worlds::Instance()->calculatePlayerLoad(world);
}

void LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();

	Worlds::Instance()->getWorld(player->getWorldId())->removeChannel(channel);
	LoginServer::Instance()->log(LogTypes::ServerDisconnect, "World " + lexical_cast<string>(static_cast<int16_t>(player->getWorldId())) + "; Channel " + lexical_cast<string>(channel));
}

void LoginServerAcceptHandler::toWorlds(LoginServerAcceptConnection *player, PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Worlds::Instance()->toWorlds(pack);
}

void LoginServerAcceptHandler::registerCashServer(LoginServerAcceptConnection *player, PacketReader &packet) {
	Worlds::Instance()->getWorld(player->getWorldId())->setCashServerConnected(true);
	LoginServer::Instance()->log(LogTypes::ServerConnect, "World " + lexical_cast<string>(static_cast<int16_t>(player->getWorldId())) + "; Cash server");
}

void LoginServerAcceptHandler::removeCashServer(LoginServerAcceptConnection *player, PacketReader &packet) {
	Worlds::Instance()->getWorld(player->getWorldId())->setCashServerConnected(false);
	LoginServer::Instance()->log(LogTypes::ServerDisconnect, "World " + lexical_cast<string>(static_cast<int16_t>(player->getWorldId())) + "; Cash server");
}
