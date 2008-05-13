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
#include "WorldServerAcceptPlayerPacket.h"
#include "WorldServerAcceptPlayer.h"
#include "PacketCreator.h"
#include "Channels.h"

void WorldServerAcceptPlayerPacket::connect(WorldServerAcceptPlayer *player, int channel, int port) {
	Packet packet = Packet();
	packet.addHeader(INTER_CHANNEL_CONNECT);
	packet.addInt(channel);
	packet.addInt(port);
	packet.packetSend(player);
}

void WorldServerAcceptPlayerPacket::channelNum(int num) {
	Packet packet = Packet();
	packet.addHeader(INTER_CHANNEL_NUM);
	packet.addInt(num);
	Channels::Instance()->sendAll(packet);
}
