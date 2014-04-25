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
#include "CommonHeader.hpp"
#include "PacketReader.hpp"
#include "PingPacket.hpp"
#include "Session.hpp"
#include "Timer.hpp"
#include "TimeUtilities.hpp"
#include <chrono>
#include <functional>
#include <iostream>

const milliseconds_t InitialPing = milliseconds_t(60000);
const milliseconds_t PingTime = milliseconds_t(15000);

AbstractConnection::AbstractConnection(bool isServer) :
	m_latency(InitialPing),
	m_lastPing(),
	m_ip(0),
	m_isServer(isServer)
{
}

auto AbstractConnection::baseHandleRequest(PacketReader &reader) -> void {
	try {
		switch (reader.peek<header_t>()) {
			case SMSG_PING:
				if (m_isServer) {
					send(PingPacket::pong());
				}
				break;
			case CMSG_PONG:
				if (m_pingCount == 0) {
					// Trying to spoof pongs without pings
					disconnect();
					return;
				}
				m_pingCount = 0;
				// This is for the trip to and from, so latency is averaged between them
				m_latency = duration_cast<milliseconds_t>(TimeUtilities::getNow() - m_lastPing) / 2;
				break;
		}
		handleRequest(reader);
	}
	catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

auto AbstractConnection::send(const PacketBuilder &builder) -> void {
	m_session->send(builder);
}

auto AbstractConnection::disconnect() -> void {
	m_session->disconnect();
}

auto AbstractConnection::getIp() const -> const Ip & {
	return m_ip;
}

auto AbstractConnection::getLatency() const -> milliseconds_t {
	return m_latency;
}

auto AbstractConnection::ping() -> void {
	if (m_doesPing) {
		if (m_pingCount == 3) {
			// We have a timeout now
			disconnect();
			return;
		}

		m_pingCount++;
		m_lastPing = TimeUtilities::getNow();
		send(PingPacket::ping());
	}
}

auto AbstractConnection::setSession(Session *val, bool ping, const Ip &ip) -> void {
	m_session = val;
	m_doesPing = ping;
	m_ip = ip;

	Timer::Timer::create([this](const time_point_t &now) { this->ping(); },
		Timer::Id(TimerType::PingTimer),
		getTimers(), InitialPing, PingTime);
}