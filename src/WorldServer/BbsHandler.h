/*
Copyright (C) 2008-2009 Vana Development Team

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

#ifndef BBSHANDLER_H
#define BBSHANDLER_H

#include "GuildBbs.h"
#include "PacketReader.h"
#include "PacketCreator.h"
#include "Types.h"
#include "WorldServerAcceptConnection.h"
#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>

using std::string;
using std::map;
using std::tr1::unordered_map;

class PacketCreator;
class PacketReader;
class WorldServerAcceptConnection;
class BbsThread;
class BbsReply;

namespace BbsHandler {
	void handlePacket(WorldServerAcceptConnection *player, PacketReader &packet);

	//Threads:
	void handleNewThread(PacketReader &packet);
	void handleDeleteThread(PacketReader &packet);

	//Replies:
	void handleNewReply(PacketReader &packet);
	void handleDeleteReply(PacketReader &packet);

	//Other:
	void handleShowThreadList(PacketReader &packet);
	void handleShowThread(PacketReader &pack);
	void handleShowThread(int32_t playerid, int32_t threadid);
};


#endif
