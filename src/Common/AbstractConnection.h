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

#include "MapleSession.h"
#include "Timer/Container.h"
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;

class PacketReader;

class AbstractConnection {
public:
	AbstractConnection();
	virtual ~AbstractConnection() { }

	virtual void realHandleRequest(PacketReader &packet) = 0;
	void handleRequest(PacketReader &packet);
	void setTimer();
	void ping();

	MapleSession * getSession() const { return m_session; }
	void setSession(MapleSession *val);
	uint32_t getIp() const { return m_ip; }
	void setIp(uint32_t ip) { m_ip = ip; }
	Timer::Container * getTimers() const { return m_timers.get(); }
	uint32_t getLatency() const { return static_cast<uint32_t>(m_latency); }

	// Times in milliseconds
	const static uint32_t InitialPing = 60000; // Shouldn't be modified much; client launching may be slow
	const static uint32_t PingTime = 15000; // Lower values (~15000) give better latency approximation but will disconnect quicker during lag
protected:
	MapleSession *m_session;
	uint32_t m_ip;
	bool m_isServer;
private:
	bool m_isPinged;
	clock_t m_latency;
	clock_t m_lastPing;
	boost::scoped_ptr<Timer::Container> m_timers;
};

class AbstractConnectionFactory {
public:
	virtual AbstractConnection * createConnection() = 0;
	virtual ~AbstractConnectionFactory() { }
};

