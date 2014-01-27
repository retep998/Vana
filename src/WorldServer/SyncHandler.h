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
#pragma once

#include "Types.h"
#include <string>

class AbstractConnection;
class PacketCreator;
class PacketReader;
class Player;

namespace SyncHandler {
	// Dispatch
	auto handle(AbstractConnection *connection, PacketReader &packet) -> void;
	// Config
	auto handleConfigSync(PacketReader &packet) -> void;
	auto handleSetRates(PacketReader &packet) -> void;
	auto handleScrollingHeader(PacketReader &packet) -> void;
	// Party
	auto handlePartySync(PacketReader &packet) -> void;
	// Player
	auto handlePlayerSync(AbstractConnection *connection, PacketReader &packet) -> void;
	auto playerConnect(int16_t channel, PacketReader &packet) -> void;
	auto playerDisconnect(int16_t channel, PacketReader &packet) -> void;
	auto playerChangeChannel(AbstractConnection *connection, PacketReader &packet) -> void;
	auto handleChangeChannel(AbstractConnection *connection, PacketReader &packet) -> void;
	auto handlePlayerUpdate(PacketReader &packet) -> void;
	auto handleCharacterCreated(PacketReader &packet) -> void;
	// Buddy
	auto handleBuddySync(PacketReader &packet) -> void;
	auto buddyInvite(PacketReader &packet) -> void;
	auto buddyOnline(PacketReader &packet) -> void;
}