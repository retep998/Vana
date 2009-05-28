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
#ifndef BUFFHOLDER_H
#define BUFFHOLDER_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

class PacketReader;

class BuffHolder : boost::noncopyable {
public:
	static BuffHolder * Instance() {
		if (singleton == 0)
			singleton = new BuffHolder;
		return singleton;
	}

	void parseIncomingBuffs(PacketReader &packet);
	void removePacket(int32_t playerid);
	bool checkPlayer(int32_t playerid);
	PacketReader & getPacket(int32_t playerid);
private:
	BuffHolder() {};
	static BuffHolder *singleton;

	unordered_map<int32_t, PacketReader> m_map;
};

#endif