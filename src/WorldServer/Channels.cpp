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
#include "Channels.h"
#include "Channel.h"
#include "LoginServerConnectPacket.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"

Channels * Channels::singleton = nullptr;

void Channels::registerChannel(WorldServerAcceptConnection *player, uint16_t channel, uint32_t ip, const IpMatrix &extIp, uint16_t port) {
	shared_ptr<Channel> chan(new Channel());
	chan->setConnection(player);
	chan->setId(channel);
	chan->setIp(ip);
	chan->setExternalIps(extIp);
	chan->setPort(port);
	channels[channel] = chan;
}

void Channels::removeChannel(uint16_t channel) {
	channels.erase(channel);
}

Channel * Channels::getChannel(uint16_t num) {
	return channels.find(num) != channels.end() ? channels[num].get() : nullptr;
}

void Channels::sendToAll(PacketCreator &packet) {
	for (unordered_map<uint16_t, shared_ptr<Channel> >::iterator iter = channels.begin(); iter != channels.end(); iter++) {
		sendToChannel(iter->first, packet);
	}
}

void Channels::sendToChannel(uint16_t channel, PacketCreator &packet) {
	getChannel(channel)->getConnection()->getSession()->send(packet);
}

void Channels::increasePopulation(uint16_t channel) {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->increasePlayers());
}

void Channels::decreasePopulation(uint16_t channel) {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->decreasePlayers());
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
