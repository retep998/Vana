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
#include "ReactorPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Reactor.h"
#include "SendHeader.h"

void ReactorPacket::spawnReactor(Reactor *reactor) {
	PacketCreator packet;
	packet.addHeader(SMSG_REACTOR_SPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int32_t>(reactor->getReactorId());
	packet.add<int8_t>(reactor->getState());
	packet.addPos(reactor->getPos());
	packet.add<int8_t>(0);
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}

void ReactorPacket::showReactor(Player *player, Reactor *reactor) {
	PacketCreator packet;
	packet.addHeader(SMSG_REACTOR_SPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int32_t>(reactor->getReactorId());
	packet.add<int8_t>(reactor->getState());
	packet.addPos(reactor->getPos());
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void ReactorPacket::triggerReactor(Reactor *reactor) {
	PacketCreator packet;
	packet.addHeader(SMSG_REACTOR_TRIGGER);
	packet.add<int32_t>(reactor->getId());
	packet.add<int8_t>(reactor->getState());
	packet.addPos(reactor->getPos());
	packet.add<int32_t>(0);
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}

void ReactorPacket::destroyReactor(Reactor *reactor) {
	PacketCreator packet;
	packet.addHeader(SMSG_REACTOR_DESPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int8_t>(reactor->getState());
	packet.addPos(reactor->getPos());
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}
