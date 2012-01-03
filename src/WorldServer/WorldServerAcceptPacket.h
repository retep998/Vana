/*
Copyright (C) 2008-2012 Vana Development Team

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

class PacketReader;
class WorldServerAcceptConnection;

namespace WorldServerAcceptPacket {
	void connect(WorldServerAcceptConnection *connection, uint16_t channel, port_t port);
	void findPlayer(WorldServerAcceptConnection *connection, int32_t finder, uint16_t channel, const string &findee, uint8_t is = 0);
	void whisperPlayer(int16_t channel, int32_t whisperee, const string &whisperer, int16_t whispererChannel, const string &message);
	void scrollingHeader(const string &message);
	void groupChat(uint16_t channel, int32_t playerId, int8_t type, const string &message, const string &sender);

	// I have my eye on you...
	void sendRates(WorldServerAcceptConnection *connection, int32_t setBit);
}