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
#include "PacketHandler.h"
#include "ReadPacket.h"
#include "PingPacket.h"
#include "Timer.h"
#include "SendHeader.h"
#include <boost/bind.hpp>

AbstractPlayer::AbstractPlayer() :
is_server(false),
is_pinged(false),
timers(new NewTimer::Container)
{
	setTimer();
}

void AbstractPlayer::handleRequest(ReadPacket *packet) {
	is_pinged = false;
	if (is_server && packet->getHeader() == SEND_PING) {
		PingPacket::pong(this);
	}
	realHandleRequest(packet);
}

void AbstractPlayer::setTimer() {
	new NewTimer::OneTimer(boost::bind(&AbstractPlayer::ping, this),
		NewTimer::OneTimer::Id(NewTimer::Types::PingTimer, 0, 0),
		getTimers(), 15000, true);
}

void AbstractPlayer::ping() {
	if (is_pinged) { // We have a timeout now
		packetHandler->disconnect();
		return;
	}
	is_pinged = true;
	PingPacket::ping(this);
}

AbstractPlayer::~AbstractPlayer() { }
