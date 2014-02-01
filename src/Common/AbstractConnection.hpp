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

class PacketBuilder;
class PacketReader;

class AbstractConnection : public TimerContainerHolder {
public:
	virtual ~AbstractConnection() = default;

	auto send(const PacketBuilder &builder) -> void;
	auto disconnect() -> void;
	auto getIp() const -> const Ip &;
	auto getLatency() const -> milliseconds_t;
protected:
	AbstractConnection(bool isServer = false);
	virtual auto handleRequest(PacketReader &reader) -> void = 0;
private:
	friend class Session;
	auto ping() -> void;
	auto setSession(Session *val, bool ping, const Ip &ip) -> void;
	auto baseHandleRequest(PacketReader &reader) -> void;

	bool m_isServer = false;
	bool m_isPinged = false;
	bool m_doesPing = true;
	Session *m_session = nullptr;
	Ip m_ip;
	milliseconds_t m_latency = milliseconds_t(0);
	time_point_t m_lastPing;
};