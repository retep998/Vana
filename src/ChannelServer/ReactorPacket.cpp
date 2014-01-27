/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Reactor.h"
#include "Session.h"
#include "SmsgHeader.h"

auto ReactorPacket::spawnReactor(Reactor *reactor) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_REACTOR_SPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int32_t>(reactor->getReactorId());
	packet.add<int8_t>(reactor->getState());
	packet.addClass<Pos>(reactor->getPos());
	packet.add<bool>(reactor->facesLeft());
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}

auto ReactorPacket::showReactor(Player *player, Reactor *reactor) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_REACTOR_SPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int32_t>(reactor->getReactorId());
	packet.add<int8_t>(reactor->getState());
	packet.addClass<Pos>(reactor->getPos());
	packet.add<bool>(reactor->facesLeft());
	player->getSession()->send(packet);
}

auto ReactorPacket::triggerReactor(Reactor *reactor) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_REACTOR_TRIGGER);
	packet.add<int32_t>(reactor->getId());
	packet.add<int8_t>(reactor->getState());
	packet.addClass<Pos>(reactor->getPos());
	packet.add<int32_t>(0);
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}

auto ReactorPacket::destroyReactor(Reactor *reactor) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_REACTOR_DESPAWN);
	packet.add<int32_t>(reactor->getId());
	packet.add<int8_t>(reactor->getState());
	packet.addClass<Pos>(reactor->getPos());
	Maps::getMap(reactor->getMapId())->sendPacket(packet);
}