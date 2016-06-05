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

#include "common_temp/Ip.hpp"
#include "common_temp/IPacket.hpp"
#include "common_temp/Types.hpp"
#include <stdexcept>
#include <string>

namespace vana {
	class packet_builder;

	class client_ip {
	public:
		explicit client_ip(const ip &ip);
	private:
		friend struct packet_serialize<client_ip>;

		client_ip() : m_ip{0} { }

		ip m_ip;
	};

	template <>
	struct packet_serialize<client_ip> {
		auto write(packet_builder &builder, const client_ip &obj) -> void {
			if (obj.m_ip.get_type() == ip::type::ipv4) {
				builder.add<uint32_t>(htonl(obj.m_ip.as_ipv4()));
			}
			else {
				throw not_implemented_exception{"i_pv6"};
			}
		}
	};
}