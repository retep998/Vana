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
#include "LoginServerAcceptHandler.h"
#include "LoginServerAcceptConnection.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Worlds.h"
#include <iostream>

void LoginServerAcceptHandler::registerChannel(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	Channel *chan = new Channel();
	chan->ip = packet.get<int32_t>();
	IpUtilities::extractExternalIp(packet, chan->external_ip);
	chan->port = packet.get<int16_t>();
	Worlds::worlds[player->getWorldId()]->channels[channel] = shared_ptr<Channel>(chan);
	std::cout << "Registering channel " << channel << " with IP " << IpUtilities::ipToString(chan->ip) << " and port " << chan->port << std::endl;
}

void LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();
	int32_t population = packet.get<int32_t>();

	Worlds::worlds[player->getWorldId()]->channels[channel]->population = population;
	Worlds::calculatePlayerLoad(Worlds::worlds[player->getWorldId()]);
}

void LoginServerAcceptHandler::removeChannel(LoginServerAcceptConnection *player, PacketReader &packet) {
	int32_t channel = packet.get<int32_t>();

	Worlds::worlds[player->getWorldId()]->channels.erase(channel);
	std::cout << "Removed channel " << channel << std::endl;
}

void LoginServerAcceptHandler::toWorlds(LoginServerAcceptConnection *player, PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Worlds::toWorlds(pack);
}
