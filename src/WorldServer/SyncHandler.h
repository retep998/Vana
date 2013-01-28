/*
Copyright (C) 2008-2013 Vana Development Team

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

using std::string;

class AbstractConnection;
class PacketCreator;
class PacketReader;
class Player;

namespace SyncHandler {
	// Dispatch
	void handle(AbstractConnection *connection, PacketReader &packet);
	// Config
	void handleConfigSync(PacketReader &packet);
	void handleSetRates(PacketReader &packet);
	void handleScrollingHeader(PacketReader &packet);
	// Party
	void handlePartySync(PacketReader &packet);
	// Player
	void handlePlayerSync(AbstractConnection *connection, PacketReader &packet);
	void playerConnect(int16_t channel, PacketReader &packet);
	void playerDisconnect(int16_t channel, PacketReader &packet);
	void playerChangeChannel(AbstractConnection *connection, PacketReader &packet);
	void handleChangeChannel(AbstractConnection *connection, PacketReader &packet);
	void handlePlayerUpdate(PacketReader &packet);
	void handleCharacterCreated(PacketReader &packet);
	// Buddy
	void handleBuddySync(PacketReader &packet);
	void buddyInvite(PacketReader &packet);
	void buddyOnline(PacketReader &packet);
}