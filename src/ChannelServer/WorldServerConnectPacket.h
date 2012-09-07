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
#include <vector>

using std::string;
using std::vector;

class PacketCreator;
class Player;
struct Rates;

namespace WorldServerConnectPacket {
	void findPlayer(int32_t playerId, const string &findeeName);
	void whisperPlayer(int32_t playerId, const string &whisperee, const string &message);
	void groupChat(int8_t type, int32_t playerId, const vector<int32_t> &receivers, const string &chat);
	void rankingCalculation();
	void sendToChannels(PacketCreator &packet);
	void sendToWorlds(PacketCreator &packet);
	void reloadMcdb(const string &type);
	void rehashConfig();
}