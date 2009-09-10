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
#include "Channels.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"

Channels * Channels::singleton = 0;

void Channels::registerChannel(WorldServerAcceptConnection *player, uint16_t channel, uint32_t ip, const vector<vector<uint32_t> > &extIp, uint16_t port) {
	shared_ptr<Channel> chan(new Channel());
	chan->player = player;
	chan->id = channel;
	chan->ip = ip;
	chan->external_ip = extIp;
	chan->port = port;
	channels[channel] = chan;
}

void Channels::removeChannel(uint16_t channel) {
	channels.erase(channel);
}

Channel * Channels::getChannel(uint16_t num) {
	return channels.find(num) != channels.end() ? channels[num].get() : 0;
}

void Channels::sendToAll(PacketCreator &packet) {
	for (unordered_map<uint16_t, shared_ptr<Channel> >::iterator iter = channels.begin(); iter != channels.end(); iter++) {
		iter->second->player->getSession()->send(packet);
	}
}

uint16_t Channels::size() {
	return channels.size();
}

uint16_t Channels::getAvailableChannel() {
	uint16_t channel = -1;
	for (uint16_t i = 0; i < (uint16_t) WorldServer::Instance()->getMaxChannels(); i++) {
		if (channels.find(i) == channels.end()) {
			channel = i;
			break;
 		}
 	}
	return channel;
}
