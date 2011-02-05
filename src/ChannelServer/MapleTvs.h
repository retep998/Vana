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

#include "PacketCreator.h"
#include "Types.h"
#include <list>
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::list;
using std::string;
using std::vector;
using std::tr1::unordered_map;

class Map;
class Player;
namespace Timer {
	class Container;
};

struct MapleTvMessage {
	MapleTvMessage() : msg1(""), msg2(""), msg3(""), msg4(""), msg5(""), time(0), megaphoneid(0), hasreceiver(false) { }
	string msg1;
	string msg2;
	string msg3;
	string msg4;
	string msg5;
	string sendname;
	string recvname;
	int32_t time;
	int32_t megaphoneid;
	int32_t senderid;
	uint32_t counter;
	bool hasreceiver;
	PacketCreator recvdisplay;
	PacketCreator senddisplay;
};

class MapleTvs : boost::noncopyable {
public:
	static MapleTvs * Instance() {
		if (singleton == nullptr)
			singleton = new MapleTvs;
		return singleton;
	}

	void addMap(Map *map);

	void addMessage(Player *sender, Player *receiver, const string &msg, const string &msg2, const string &msg3, const string &msg4, const string &msg5, int32_t megaphoneid, int32_t time);
	void getMapleTvEntryPacket(PacketCreator &packet);
	bool isMapleTvMap(int32_t id) const { return (m_maps.find(id) != m_maps.end()); }
	bool hasMessage() const { return m_hasmessage; }
	uint32_t getCounter() { return ++m_counter; }
private:
	MapleTvs();
	static MapleTvs *singleton;

	void parseBuffer();
	void getMapleTvPacket(MapleTvMessage &message, PacketCreator &packet, int32_t timeleft = 0);
	void endMapleTvPacket(PacketCreator &packet);
	void sendPacket(PacketCreator &packet);
	int32_t checkMessageTimer() const;
	Timer::Container * getTimers() const { return m_timers.get(); }

	bool m_hasmessage;
	uint32_t m_counter;
	list<MapleTvMessage> m_buffer;
	unordered_map<int32_t, Map *> m_maps;
	boost::scoped_ptr<Timer::Container> m_timers;
	MapleTvMessage m_currentmessage;
};
