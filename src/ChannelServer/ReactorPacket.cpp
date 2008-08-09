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
#include "Maps.h"

void ReactorPacket::spawnReactor(Reactor *reactor) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_REACTOR);
	packet.addInt(reactor->getID());
	packet.addInt(reactor->getReactorID());
	packet.addByte(reactor->getState());
	packet.addPos(reactor->getPos());
	packet.addByte(0);
	Maps::maps[reactor->getMapID()]->sendPacket(packet);
}

void ReactorPacket::showReactor(Player *player, Reactor *reactor) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_REACTOR);
	packet.addInt(reactor->getID());
	packet.addInt(reactor->getReactorID());
	packet.addByte(reactor->getState());
	packet.addPos(reactor->getPos());
	packet.addByte(0);
	player->getPacketHandler()->sendPacket(packet);
}

void ReactorPacket::triggerReactor(Reactor *reactor) {
	PacketCreator packet = PacketCreator();
	packet.addShort(SEND_TRIGGER_REACTOR);
	packet.addInt(reactor->getID());
	packet.addByte(reactor->getState()); // State
	packet.addPos(reactor->getPos());
	packet.addInt(0);
	Maps::maps[reactor->getMapID()]->sendPacket(packet);
}

void ReactorPacket::destroyReactor(Reactor *reactor) {
	PacketCreator packet = PacketCreator();
	packet.addShort(SEND_DESTROY_REACTOR);
	packet.addInt(reactor->getID());
	packet.addByte(reactor->getState());
	packet.addPos(reactor->getPos());
	Maps::maps[reactor->getMapID()]->sendPacket(packet);
}
