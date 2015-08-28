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
#include <string>
#include <vector>

namespace Vana {
	class PacketBuilder;
	class PacketReader;

	class ExternalIp : public Ip {
	public:
		ExternalIp(const string_t &addr, uint32_t ipv4SubnetMask);
		ExternalIp(uint32_t ipv4, uint32_t ipv4SubnetMask);

		auto tryMatchIpToSubnet(const Ip &test, Ip &result) const -> bool;
	private:
		friend struct PacketSerialize<ExternalIp>;
		ExternalIp() : Ip{} { }

		uint32_t m_ipv4SubnetMask = 0;
	};

	template <>
	struct PacketSerialize<ExternalIp> {
		auto read(PacketReader &reader) -> ExternalIp {
			ExternalIp ret;
			ret.m_type = reader.get<Ip::Type>();
			if (ret.m_type == Ip::Type::Ipv4) {
				ret.m_ipv4 = reader.get<uint32_t>();
				ret.m_ipv4SubnetMask = reader.get<uint32_t>();
			}
			return ret;
		}
		auto write(PacketBuilder &builder, const ExternalIp &obj) -> void {
			builder.add<Ip::Type>(obj.m_type);
			if (obj.m_type == Ip::Type::Ipv4) {
				builder.add<uint32_t>(obj.m_ipv4);
				builder.add<uint32_t>(obj.m_ipv4SubnetMask);
			}
		}
	};

	using IpMatrix = vector_t<ExternalIp>;
}