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

class WorldServerAcceptPlayer;
class PacketReader;

namespace WorldServerAcceptHandler {
	void playerChangeChannel(WorldServerAcceptPlayer *player, PacketReader &packet);
	void findPlayer(WorldServerAcceptPlayer *player, PacketReader &packet);
	void whisperPlayer(WorldServerAcceptPlayer *player, PacketReader &packet);
	void registerPlayer(WorldServerAcceptPlayer *player, PacketReader &packet);
	void removePlayer(WorldServerAcceptPlayer *player, PacketReader &packet);
	void scrollingHeader(WorldServerAcceptPlayer *player, PacketReader &packet);
	void partyOperation(WorldServerAcceptPlayer *player, PacketReader &packet);
	void updateLevel(WorldServerAcceptPlayer *player, PacketReader &packet);
	void updateJob(WorldServerAcceptPlayer *player, PacketReader &packet);
	void updateMap(WorldServerAcceptPlayer *player, PacketReader &packet);
	void groupChat(WorldServerAcceptPlayer *player, PacketReader &packet);
}

#endif
