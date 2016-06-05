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

#include "common/Ip.hpp"
#include "common/IPacket.hpp"
#include "common/Types.hpp"
#include <string>
#include <vector>

namespace vana {
	class packet_builder;
	class packet_reader;

	class external_ip : public ip {
	public:
		external_ip(const string &addr, uint32_t ipv4_subnet_mask);
		external_ip(uint32_t ipv4, uint32_t ipv4_subnet_mask);

		auto try_match_ip_to_subnet(const ip &test, ip &result) const -> bool;
	private:
		friend struct packet_serialize<external_ip>;
		external_ip() : ip{} { }

		uint32_t m_ipv4_subnet_mask = 0;
	};

	template <>
	struct packet_serialize<external_ip> {
		auto read(packet_reader &reader) -> external_ip {
			external_ip ret;
			ret.m_type = reader.get<ip::type>();
			if (ret.m_type == ip::type::ipv4) {
				ret.m_ipv4 = reader.get<uint32_t>();
				ret.m_ipv4_subnet_mask = reader.get<uint32_t>();
			}
			return ret;
		}
		auto write(packet_builder &builder, const external_ip &obj) -> void {
			builder.add<ip::type>(obj.m_type);
			if (obj.m_type == ip::type::ipv4) {
				builder.add<uint32_t>(obj.m_ipv4);
				builder.add<uint32_t>(obj.m_ipv4_subnet_mask);
			}
		}
	};

	using ip_matrix = vector<external_ip>;
}