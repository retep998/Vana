/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class PacketHandler {
	public:
		auto getIp() const -> optional_t<Ip>;
		auto disconnect() -> void;
		auto send(const PacketBuilder &builder) -> void;
		auto getLatency() const -> milliseconds_t;
	protected:
		friend class Session;
		virtual auto handle(PacketReader &reader) -> Result;
		virtual auto onConnect() -> void;
		virtual auto onDisconnect() -> void;
		auto onConnectBase(ref_ptr_t<Session> session) -> void;
		auto onDisconnectBase() -> void;

		bool m_disconnected = false;
		ref_ptr_t<Session> m_session;
	};
}