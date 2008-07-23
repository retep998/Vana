/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.d

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "PingPacket.h"
#include "AbstractPlayer.h"
#include "PacketCreator.h"
#include "SendHeader.h"
#include "RecvHeader.h"

void PingPacket::ping(AbstractPlayer *player) {
	PacketCreator packet;
	packet.addHeader(SEND_PING);
	packet.send(player);
}

void PingPacket::pong(AbstractPlayer *player) {
	PacketCreator packet;
	packet.addHeader(RECV_PONG);
	packet.send(player);
}
