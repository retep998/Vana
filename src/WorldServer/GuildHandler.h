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
#ifndef GUILDHANDLER_H
#define GUILDHANDLER_H

#include "Types.h"
#include <string>

using std::string;

class LoginServerConnection;
class PacketCreator;
class PacketReader;
class WorldServerAcceptConnection;

namespace GuildHandler {
	void handlePacket(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet);

	void loadGuild(int32_t id);
	void handleGuildCreation(PacketReader &packet);

	void sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled);
	void sendGuildInvite(int32_t guildid, PacketReader &packet);
	void sendNewPlayer(int32_t guildid, int32_t pid, bool newGuild = true);
	void sendUpdateOfTitles(int32_t guildid, PacketReader &packet);
	void sendGuildNotice(int32_t guildid, PacketReader &packet);
};

#endif
