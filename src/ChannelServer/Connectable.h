/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "Ip.h"
#include "noncopyable.hpp"
#include "Types.h"
#include <memory>
#include <unordered_map>

class PacketReader;

using std::unordered_map;

struct ConnectingPlayer {
	Ip connectIp;
	time_point_t connectTime;
	std::shared_ptr<PacketReader> heldPacket;

	ConnectingPlayer() : connectIp(0) { }
};

typedef unordered_map<int32_t, ConnectingPlayer> ConnectableMap;

class Connectable : boost::noncopyable {
public:
	static Connectable * Instance() {
		if (singleton == nullptr)
			singleton = new Connectable;
		return singleton;
	}

	void newPlayer(int32_t id, const Ip &ip, PacketReader &packet);
	bool checkPlayer(int32_t id, const Ip &ip);
	PacketReader * getPacket(int32_t id);
	void playerEstablished(int32_t id);
private:
	Connectable() {}
	static Connectable *singleton;

	const static uint32_t MaxMilliseconds = 5000;

	ConnectableMap m_map;
};