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
#include "SyncPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Session.h"
#include "World.h"

auto SyncPacket::PlayerPacket::characterCreated(World *world, int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::CharacterCreated);
	packet.add<int32_t>(playerId);
	world->send(packet);
}