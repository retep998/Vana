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
#include "BufferUtilities.h"
#include "Worlds.h"
#include <iostream>

void LoginServerAcceptHandler::registerChannel(LoginServerAcceptPlayer *player, unsigned char *packet) {
	int channel = BufferUtilities::getInt(packet);
	short iplen = BufferUtilities::getShort(packet+4);
	Channel *chan = new Channel();
	BufferUtilities::getString(packet+6, iplen, chan->ip);
	chan->port = BufferUtilities::getShort(packet+6+iplen);
	Worlds::worlds[player->getWorldId()]->channels[channel] = chan;
	std::cout << "Registering channel " << channel << " with IP " << chan->ip << " and port " << chan->port << std::endl;
}
