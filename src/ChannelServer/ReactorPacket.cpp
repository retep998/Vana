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
#include "ReactorPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Reactors.h"
#include "SendHeader.h"

void ReactorPacket::showReactor(Player* player, ReactorInfo reactor, int i) {
	Packet packet;
	packet.addHeader(SEND_SHOW_REACTOR);
	packet.addInt(i+0x64);
	packet.addInt(reactor.id);
	packet.addByte(0);
	packet.addShort(reactor.x);
	packet.addShort(reactor.y);
	packet.addByte(reactor.f);
	packet.send(player);
}
