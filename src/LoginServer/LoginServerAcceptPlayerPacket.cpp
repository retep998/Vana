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
#include "LoginServerAcceptPlayerPacket.h"
#include "LoginServerAcceptPlayer.h"
#include "PacketCreator.h"

void LoginServerAcceptPlayerPacket::connect(LoginServerAcceptPlayer *player, char worldid, short port, int maxchan) {
	Packet packet;
	packet.addHeader(INTER_WORLD_CONNECT);
	packet.addByte(worldid);
	packet.addShort(port);
	packet.addInt(maxchan);
	packet.send(player);
}

void LoginServerAcceptPlayerPacket::connectChannel(LoginServerAcceptPlayer *player, char worldid, const string &ip, short port, int exprate, int questexprate, int mesorate, int droprate) {
	Packet packet;
	packet.addHeader(INTER_LOGIN_CHANNEL_CONNECT);
	packet.addByte(worldid);
	packet.addString(ip);
	packet.addShort(port);
	packet.addInt(exprate);
	packet.addInt(questexprate);
	packet.addInt(mesorate);
	packet.addInt(droprate);
	packet.send(player);
}

void LoginServerAcceptPlayerPacket::newPlayer(LoginServerAcceptPlayer *player, int channel, int charid) {
	Packet packet;
	packet.addHeader(INTER_NEW_PLAYER);
	packet.addInt(channel);
	packet.addInt(charid);
	packet.send(player);
}
