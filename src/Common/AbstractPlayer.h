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
#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include "MapleSession.h"
#include "Timer/Container.h"
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;

class PacketReader;

class AbstractPlayer {
public:
	AbstractPlayer();
	virtual ~AbstractPlayer() { }
	
	virtual void realHandleRequest(PacketReader &packet) = 0;
	void handleRequest(PacketReader &packet);
	void setTimer();
	void ping();

	MapleSession * getSession() const { return m_session; }
	void setSession(MapleSession *val);
	uint32_t getIp() const { return m_ip; }
	void setIp(uint32_t ip) { m_ip = ip; }
	Timer::Container * getTimers() const { return m_timers.get(); }
protected:
	MapleSession *m_session;
	uint32_t m_ip;
	bool m_is_server;
private:
	bool m_is_pinged;
	boost::scoped_ptr<Timer::Container> m_timers;
};

class AbstractPlayerFactory {
public:
	virtual AbstractPlayer * createPlayer () = 0;
	virtual ~AbstractPlayerFactory() { }
};
#endif
