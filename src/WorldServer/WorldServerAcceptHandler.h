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
#pragma once

class WorldServerAcceptConnection;
class PacketReader;

namespace WorldServerAcceptHandler {
	void findPlayer(WorldServerAcceptConnection *player, PacketReader &packet);
	void whisperPlayer(WorldServerAcceptConnection *player, PacketReader &packet);
	void scrollingHeader(WorldServerAcceptConnection *player, PacketReader &packet);
	void groupChat(WorldServerAcceptConnection *player, PacketReader &packet);
	void sendToChannels(PacketReader &packet);
	void sendToLogin(PacketReader &packet);
	void sendToCashServer(PacketReader &packet);
}
