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
#include "LoginServerConnectPacket.h"
#include "InterHeader.h"
#include "LoginServerConnectPlayer.h"
#include "MapleSession.h"
#include "PacketCreator.h"

void LoginServerConnectPacket::registerChannel(LoginServerConnectPlayer *player, int32_t channel, const string &ip, int16_t port) {
	PacketCreator packet;
	packet.addShort(INTER_REGISTER_CHANNEL);
	packet.addInt(channel);
	packet.addString(ip);
	packet.addShort(port);
	player->getSession()->send(packet);
}

void LoginServerConnectPacket::updateChannelPop(LoginServerConnectPlayer *player, int32_t channel, int32_t population) {
	PacketCreator packet;
	packet.addShort(INTER_UPDATE_CHANNEL_POP);
	packet.addInt(channel);
	packet.addInt(population);
	
	player->getSession()->send(packet);
}

void LoginServerConnectPacket::removeChannel(LoginServerConnectPlayer *player, int32_t channel) {
	PacketCreator packet;
	packet.addShort(INTER_REMOVE_CHANNEL);
	packet.addInt(channel);

	player->getSession()->send(packet);
}