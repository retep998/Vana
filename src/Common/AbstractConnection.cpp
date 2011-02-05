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
#include "AbstractConnection.h"
#include "MapleSession.h"
#include "PacketReader.h"
#include "PingPacket.h"
#include "RecvHeader.h"
#include "SendHeader.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <iostream>
#include <functional>

using std::tr1::bind;

AbstractConnection::AbstractConnection() :
m_isServer(false),
m_isPinged(false),
m_latency(InitialPing),
m_timers(new Timer::Container)
{
}

void AbstractConnection::handleRequest(PacketReader &packet) {
	try {
		switch (packet.getHeader(false)) {
			case SMSG_PING:
				if (m_isServer) {
					PingPacket::pong(this);
				}
				break;
			case CMSG_PONG:
				m_isPinged = false;
				m_latency = (clock() - m_lastPing) / 2; // This is for the trip to and from, so latency is half
				break;
		}
		realHandleRequest(packet);
	}
	catch (std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}
}

void AbstractConnection::setTimer() {
	new Timer::Timer(bind(&AbstractConnection::ping, this),
		Timer::Id(Timer::Types::PingTimer, 0, 0),
		getTimers(), Timer::Time::fromNow(InitialPing), PingTime);
}

void AbstractConnection::ping() {
	if (m_isPinged) {
		// We have a timeout now
		getSession()->disconnect();
		return;
	}
	m_isPinged = true;
	m_lastPing = clock();
	PingPacket::ping(this);
}

void AbstractConnection::setSession(MapleSession *val) {
	m_session = val;
	setTimer();
}
