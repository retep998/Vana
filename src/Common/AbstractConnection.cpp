/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "AbstractConnection.hpp"
#include "CmsgHeader.hpp"
#include "PacketReader.hpp"
#include "PingPacket.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "Timer.hpp"
#include "TimeUtilities.hpp"
#include <chrono>
#include <functional>
#include <iostream>

const milliseconds_t InitialPing = milliseconds_t(60000);
const milliseconds_t PingTime = milliseconds_t(15000);

AbstractConnection::AbstractConnection() :
	m_latency(InitialPing),
	m_lastPing(),
	m_ip(0)
{
}

auto AbstractConnection::baseHandleRequest(PacketReader &packet) -> void {
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
				// This is for the trip to and from, so latency is averaged between them
				m_latency = duration_cast<milliseconds_t>(TimeUtilities::getNow() - m_lastPing) / 2;
				break;
		}
		handleRequest(packet);
	}
	catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

auto AbstractConnection::getSession() const -> Session * {
	return m_session;
}

auto AbstractConnection::getIp() const -> const Ip & {
	return m_ip;
}

auto AbstractConnection::getLatency() const -> milliseconds_t {
	return m_latency;
}

auto AbstractConnection::setTimer() -> void {
	Timer::Timer::create([this](const time_point_t &now) { this->ping(); },
		Timer::Id(Timer::Types::PingTimer, 0, 0),
		getTimers(), InitialPing, PingTime);
}

auto AbstractConnection::ping() -> void {
	if (m_doesPing) {
		if (m_isPinged) {
			// We have a timeout now
			getSession()->disconnect();
			return;
		}

		m_isPinged = true;
		m_lastPing = TimeUtilities::getNow();
		PingPacket::ping(this);
	}
}

auto AbstractConnection::setSession(Session *val) -> void {
	m_session = val;
	setTimer();
}