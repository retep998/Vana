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
#ifndef WORLDSERVERACCEPTHANDLER_H
#define WORLDSERVERACCEPTHANDLER_H

#include <string>

using std::string;

class WorldServerAcceptPlayer;
class ReadPacket;

namespace WorldServerAcceptHandler {
	void playerChangeChannel(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void findPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void whisperPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void registerPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void removePlayer(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void scrollingHeader(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void partyOperation(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void updateLevel(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void updateJob(WorldServerAcceptPlayer *player, ReadPacket *packet);
	void updateMap(WorldServerAcceptPlayer *player, ReadPacket *packet);
}

#endif
