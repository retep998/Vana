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

class PingTimer: public Timer::TimerHandler {
public:
	static PingTimer * Instance() {
		if (singleton == 0)
			singleton = new PingTimer;
		return singleton;
	}

	int set(AbstractPlayer *player) {
		int id = Timer::Instance()->setTimer(15000, this, true);
		timers[id] = player;
		return id;
	}

	void reset(int id) {
		Timer::Instance()->resetTimer(id);
	}
private:
	static PingTimer *singleton;
	PingTimer() {};
	PingTimer(const PingTimer&);
	PingTimer& operator=(const PingTimer&);

	hash_map <int, AbstractPlayer *> timers;

	void handle(Timer *timer, int id) {
		if (timers.find(id) == timers.end())
			return;

		timers[id]->ping();
	}

	void remove(int id) {
		timers.erase(id);
	}
};

PingTimer * PingTimer::singleton = 0;

AbstractPlayer::AbstractPlayer() {
	is_server = false;
	is_pinged = false;
	setTimer();
}

void AbstractPlayer::handleRequest(ReadPacket *packet) {
	is_pinged = false;
	if (is_server && packet->getHeader() == SEND_PING)
		PingPacket::pong(this);
	realHandleRequest(packet);
}

void AbstractPlayer::setTimer() {
	timer = PingTimer::Instance()->set(this);
}

void AbstractPlayer::ping() {
	if (is_pinged) { // We have a timeout now
		packetHandler->disconnect();
		return;
	}
	is_pinged = true;
	PingPacket::ping(this);
}

AbstractPlayer::~AbstractPlayer() {
	Timer::Instance()->cancelTimer(timer);
}
