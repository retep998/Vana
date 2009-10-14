/*
Copyright (C) 2009 Vana Development Team

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
#ifndef ALLIANCES_H
#define ALLIANCES_H

#include "Alliance.h"
#include "Players.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp> 
#include <string>

using std::tr1::unordered_map;
using std::string;

class Guild;
class PacketCreator;

class Alliances : boost::noncopyable {
public:
	static Alliances * Instance() {
		if (singleton == 0)
			singleton = new Alliances;
		return singleton;
	}

	Alliance * getAlliance(int32_t id);
	void removeAlliance(int32_t id);
	void addAlliance(int32_t id, string name, string notice, string title1, string title2, string title3, string title4, string title5, int32_t capacity, int32_t leader);

	void getChannelConnectPacket(PacketCreator &packet);
private:
	Alliances() {};
	static Alliances *singleton;

	unordered_map<int32_t, Alliance *> m_alliances;
};

#endif
