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
#include "Channels.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "WorldServerAcceptPlayer.h"

Channels * Channels::singleton = 0;

void Channels::registerChannel(WorldServerAcceptPlayer *player, uint16_t channel, const string &ip, uint16_t port) {
	Channel *chan = new Channel();
	chan->player = player;
	chan->id = channel;
	chan->ip = ip;
	chan->port = port;
	channels[channel] = shared_ptr<Channel>(chan);
}

void Channels::removeChannel(uint16_t channel) {
	channels.erase(channel);
}

Channel * Channels::getChannel(uint16_t num) {
	if (channels.find(num) != channels.end()) {
		return channels[num].get();
	}
	else {
		return 0;
	}
}

void Channels::sendToAll(PacketCreator &packet) {
	for (unordered_map<uint16_t, shared_ptr<Channel>>::iterator iter = channels.begin(); iter != channels.end(); iter++) {
		iter->second->player->getSession()->send(packet);
	}
}

uint16_t Channels::size() {
	return channels.size();
}
