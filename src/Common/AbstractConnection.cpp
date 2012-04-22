/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "CmsgHeader.h"
#include "PacketReader.h"
#include "PingPacket.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "Timer.h"
#include "TimeUtilities.h"
#include <functional>
#include <iostream>

using std::bind;

AbstractConnection::AbstractConnection() :
	m_isServer(false),
	m_isPinged(false),
	m_doesPing(true),
	m_latency(InitialPing),
	m_lastPing(InitialPing),
	m_timers(new Timer::Container)
{
}

void AbstractConnection::baseHandleRequest(PacketReader &packet) {
	try {
		switch (packet.getHeader(false)) {
			case SMSG_PING:
				if (m_isServer) {
					PingPacket::pong(this);
				}
				break;
			case CMSG_PONG:
				if (!m_isPinged) {
					// Trying to spoof pongs without pings
					getSession()->disconnect();
					return;
				}
				m_isPinged = false;
				m_latency = (clock() - m_lastPing) / 2; // This is for the trip to and from, so latency is averaged between them
				break;
		}
		handleRequest(packet);
	}
	catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void AbstractConnection::setTimer() {
	new Timer::Timer(bind(&AbstractConnection::ping, this),
		Timer::Id(Timer::Types::PingTimer, 0, 0),
		getTimers(), TimeUtilities::fromNow(InitialPing), PingTime);
}

void AbstractConnection::ping() {
	if (m_doesPing) {
		if (m_isPinged) {
			// We have a timeout now
			getSession()->disconnect();
			return;
		}

 		m_isPinged = true;
		m_lastPing = clock();
		PingPacket::ping(this);
	}
}

void AbstractConnection::setSession(Session *val) {
	m_session = val;
	setTimer();
}