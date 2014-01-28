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
#pragma once

#include "Session.hpp"
#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include <string>

class PacketReader;

class AbstractConnection : public TimerContainerHolder {
public:
	AbstractConnection();
	virtual ~AbstractConnection() = default;

	auto getSession() const -> Session * { return m_session; }
	auto getIp() const -> const Ip & { return m_ip; }
	auto getLatency() const -> milliseconds_t { return m_latency; }
protected:
	virtual auto handleRequest(PacketReader &packet) -> void = 0;
	bool m_isServer = false;
	bool m_doesPing = true;
	Session *m_session = nullptr;
	Ip m_ip;
private:
	friend class Session;
	auto ping() -> void;
	auto setTimer() -> void;
	auto setSession(Session *val) -> void;
	auto setIp(const Ip &ip) -> void { m_ip = ip; }
	auto setPinging(bool ping) -> void { m_doesPing = ping; }
	auto baseHandleRequest(PacketReader &packet) -> void;

	bool m_isPinged = false;
	milliseconds_t m_latency = milliseconds_t(0);
	time_point_t m_lastPing;
};