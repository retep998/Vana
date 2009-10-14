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

#ifndef GUILDS_H
#define GUILDS_H

#include "Guild.h"
#include "Players.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;

class Guild;
class PacketCreator;

class Guilds : boost::noncopyable {
public:
	static Guilds * Instance() {
		if (singleton == 0)
			singleton = new Guilds;
		return singleton;
	}

	Guild * addGuild(Guild * guild);
	void createGuild(string name, string notice, int32_t id, int32_t leaderid, int32_t capacity, int16_t logo, uint8_t logocolor, int16_t logobg, uint8_t logobgcolor, int32_t gp, string title1, string title2, string title3, string title4, string title5, int32_t alliance);

	void removeGuild(Guild * guild);
	Guild * getGuild(int32_t id);
	Guild * getGuild(const string &name);

	void getChannelConnectPacket(PacketCreator &packet);
private:
	Guilds() {};
	static Guilds *singleton;

	unordered_map<int32_t, Guild *> m_guilds;
	unordered_map<string, Guild *> m_guilds_names;
};

#endif
