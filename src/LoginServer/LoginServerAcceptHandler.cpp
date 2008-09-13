/*
Copyright (C) 2008 Vana Development Team

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
#include "LoginServerAcceptPlayer.h"
#include "Worlds.h"
#include "ReadPacket.h"
#include <iostream>

void LoginServerAcceptHandler::registerChannel(LoginServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t channel = packet->getInt();
	Channel *chan = new Channel();
	chan->ip = packet->getString();
	chan->port = packet->getShort();
	Worlds::worlds[player->getWorldId()]->channels[channel] = shared_ptr<Channel>(chan);
	std::cout << "Registering channel " << channel << " with IP " << chan->ip << " and port " << chan->port << std::endl;
}

void LoginServerAcceptHandler::updateChannelPop(LoginServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t channel = packet->getInt();
	int32_t population = packet->getInt();

	Worlds::worlds[player->getWorldId()]->channels[channel]->population = population;
}

void LoginServerAcceptHandler::removeChannel(LoginServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t channel = packet->getInt();

	Worlds::worlds[player->getWorldId()]->channels.erase(channel);
	std::cout << "Removed channel " << channel << std::endl;
}
