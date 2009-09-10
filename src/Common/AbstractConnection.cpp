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
#include "AbstractConnection.h"
#include "MapleSession.h"
#include "PingPacket.h"
#include "PacketReader.h"
#include "SendHeader.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

AbstractConnection::AbstractConnection() :
m_is_server(false),
m_is_pinged(false),
m_timers(new Timer::Container)
{
}

void AbstractConnection::handleRequest(PacketReader &packet) {
	try {
		m_is_pinged = false;
		if (m_is_server && packet.getHeader() == SEND_PING) {
			PingPacket::pong(this);
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
		getTimers(), Timer::Time::fromNow(60000), 15000); // Set the initial ping to 1 minutes for some people with slow computers
}

void AbstractConnection::ping() {
	if (m_is_pinged) { // We have a timeout now
		getSession()->disconnect();
		return;
	}
	m_is_pinged = true;
	PingPacket::ping(this);
}

void AbstractConnection::setSession(MapleSession *val) {
	m_session = val;
	setTimer();
}
