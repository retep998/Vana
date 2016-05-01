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

namespace Vana {

ExternalIp::ExternalIp(const string_t &addr, uint32_t ipv4SubnetMask) :
	Ip{addr, Ip::Type::Ipv4},
	m_ipv4SubnetMask{ipv4SubnetMask}
{
}

ExternalIp::ExternalIp(uint32_t ipv4, uint32_t ipv4SubnetMask) :
	Ip{ipv4},
	m_ipv4SubnetMask{ipv4SubnetMask}
{
}

auto ExternalIp::tryMatchIpToSubnet(const Ip &test, Ip &result) const -> bool {
	if (test.m_type != m_type) throw std::invalid_argument{"IP type must match the external IP type"};

	if (m_type == Ip::Type::Ipv4) {
		if ((m_ipv4 & m_ipv4SubnetMask) == (test.m_ipv4 & m_ipv4SubnetMask)) {
			result = Ip(m_ipv4);
			return true;
		}
		return false;
	}

	return false;
}

}