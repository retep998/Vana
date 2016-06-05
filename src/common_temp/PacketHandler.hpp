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

#include "common_temp/PacketReader.hpp"
#include "common_temp/Session.hpp"
#include "common_temp/Types.hpp"

namespace vana {
	class packet_handler {
	public:
		auto get_ip() const -> optional<ip>;
		auto disconnect() -> void;
		auto send(const packet_builder &builder) -> void;
		auto get_latency() const -> milliseconds;
	protected:
		friend class session;
		virtual auto handle(packet_reader &reader) -> result;
		virtual auto on_connect() -> void;
		virtual auto on_disconnect() -> void;
		auto on_connect_base(ref_ptr<session> session) -> void;
		auto on_disconnect_base() -> void;

		bool m_disconnected = false;
		ref_ptr<session> m_session;
	};
}