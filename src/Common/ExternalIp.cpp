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
#include "ExternalIp.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include <stdexcept>

namespace vana {

external_ip::external_ip(const string &addr, uint32_t ipv4_subnet_mask) :
	ip{addr, ip::type::ipv4},
	m_ipv4_subnet_mask{ipv4_subnet_mask}
{
}

external_ip::external_ip(uint32_t ipv4, uint32_t ipv4_subnet_mask) :
	ip{ipv4},
	m_ipv4_subnet_mask{ipv4_subnet_mask}
{
}

auto external_ip::try_match_ip_to_subnet(const ip &test, ip &result) const -> bool {
	if (test.m_type != m_type) throw std::invalid_argument{"IP type must match the external IP type"};

	if (m_type == ip::type::ipv4) {
		if ((m_ipv4 & m_ipv4_subnet_mask) == (test.m_ipv4 & m_ipv4_subnet_mask)) {
			result = ip(m_ipv4);
			return true;
		}
		return false;
	}

	return false;
}

}