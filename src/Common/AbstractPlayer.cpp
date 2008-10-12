/*
Copyright (C) 2008 Vana Development Team

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
#include "AbstractPlayer.h"
#include "MapleSession.h"
#include "PingPacket.h"
#include "PacketReader.h"
#include "SendHeader.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

AbstractPlayer::AbstractPlayer() :
is_server(false),
is_pinged(false),
timers(new Timer::Container)
{
}

void AbstractPlayer::handleRequest(PacketReader &packet) {
	try {
		is_pinged = false;
		if (is_server && packet.getHeader() == SEND_PING) {
			PingPacket::pong(this);
		}
		realHandleRequest(packet);
	}
	catch (std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}
}

void AbstractPlayer::setTimer() {
	new Timer::Timer(bind(&AbstractPlayer::ping, this),
		Timer::Id(Timer::Types::PingTimer, 0, 0),
		getTimers(), 15000, true);
}

void AbstractPlayer::ping() {
	if (is_pinged) { // We have a timeout now
		session->disconnect();
		return;
	}
	is_pinged = true;
	PingPacket::ping(this);
}

void AbstractPlayer::setSession(MapleSession *val) {
	session = val;
	setTimer();
}
