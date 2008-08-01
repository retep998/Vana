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
#include "LoginServerConnectPlayerPacket.h"
#include "LoginServerConnectPlayer.h"
#include "PacketCreator.h"

void LoginServerConnectPlayerPacket::registerChannel(LoginServerConnectPlayer *player, int channel, const string &ip, short port) {
	PacketCreator packet;
	packet.addShort(INTER_REGISTER_CHANNEL);
	packet.addInt(channel);
	packet.addString(ip);
	packet.addShort(port);
	packet.send(player);
}

void LoginServerConnectPlayerPacket::updateChannelPop(LoginServerConnectPlayer *player, int channel, int population) {
	PacketCreator packet;
	packet.addShort(INTER_UPDATE_CHANNEL_POP);
	packet.addInt(channel);
	packet.addInt(population);
	
	packet.send(player);
}

void LoginServerConnectPlayerPacket::removeChannel(LoginServerConnectPlayer *player, int channel) {
	PacketCreator packet;
	packet.addShort(INTER_REMOVE_CHANNEL);
	packet.addInt(channel);

	packet.send(player);
}