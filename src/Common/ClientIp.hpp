/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Ip.hpp"
#include "IPacket.hpp"
#include "Types.hpp"
#include <stdexcept>
#include <string>

namespace Vana {
	class PacketBuilder;

	class ClientIp {
	public:
		explicit ClientIp(const Ip &ip);
	private:
		friend struct PacketSerialize<ClientIp>;

		ClientIp() : m_ip{0} { }

		Ip m_ip;
	};

	template <>
	struct PacketSerialize<ClientIp> {
		auto write(PacketBuilder &builder, const ClientIp &obj) -> void {
			if (obj.m_ip.getType() == Ip::Type::Ipv4) {
				builder.add<uint32_t>(htonl(obj.m_ip.asIpv4()));
			}
			else {
				throw NotImplementedException{"IPv6"};
			}
		}
	};
}