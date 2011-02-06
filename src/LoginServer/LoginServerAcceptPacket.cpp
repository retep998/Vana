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
#include "LoginServerAcceptPacket.h"
#include "InterHeader.h"
#include "LoginServerAcceptConnection.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Rates.h"
#include "Worlds.h"

void LoginServerAcceptPacket::connect(LoginServerAcceptConnection *player, World *world) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_WORLD_CONNECT);
	packet.add<int8_t>(world->id);
	packet.add<int16_t>(world->port);
	packet.add<int32_t>(world->maxChannels);
	packet.add<int32_t>(world->maxChars);
	packet.add<int8_t>(world->maxMultiLevel);
	packet.add<int16_t>(world->maxStats);
	packet.addString(world->scrollingHeader);
	// Boss stuff
	packet.add<int16_t>(world->pianusAttempts);
	packet.add<int16_t>(world->papAttempts);
	packet.add<int16_t>(world->zakumAttempts);
	packet.add<int16_t>(world->horntailAttempts);
	packet.add<int16_t>(world->pinkbeanAttempts);
	packet.addVector(world->pianusChannels);
	packet.addVector(world->papChannels);
	packet.addVector(world->zakumChannels);
	packet.addVector(world->horntailChannels);
	packet.addVector(world->pinkbeanChannels);
	// Rates
	packet.add<int32_t>(Rates::SetBits::all);
	packet.add<int32_t>(world->exprate);
	packet.add<int32_t>(world->questexprate);
	packet.add<int32_t>(world->mesorate);
	packet.add<int32_t>(world->droprate);
	player->getSession()->send(packet);
}

void LoginServerAcceptPacket::noMoreWorld(LoginServerAcceptConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_WORLD_CONNECT);
	packet.add<int8_t>(-1);
	player->getSession()->send(packet);
}

void LoginServerAcceptPacket::connectChannel(LoginServerAcceptConnection *player, int8_t worldid, uint32_t ip, int16_t port) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_LOGIN_CHANNEL_CONNECT);
	packet.add<int8_t>(worldid);
	packet.add<uint32_t>(ip);
	packet.add<int16_t>(port);
	player->getSession()->send(packet);
}

void LoginServerAcceptPacket::newPlayer(LoginServerAcceptConnection *player, uint16_t channel, int32_t charid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_NEW_PLAYER);
	packet.add<int16_t>(channel);
	packet.add<int32_t>(charid);
	player->getSession()->send(packet);
}
