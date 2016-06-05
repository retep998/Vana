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
#include "ExternalIpResolver.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include <stdexcept>

namespace vana {

external_ip_resolver::external_ip_resolver(const ip &default_ip, const ip_matrix &external_ips) :
	m_default_ip{default_ip},
	m_external_ips{external_ips}
{
}

auto external_ip_resolver::match_ip_to_subnet(const ip &test) const -> ip {
	if (test.get_type() != m_default_ip.get_type()) throw std::invalid_argument{"IP type must match the external IP type"};

	ip ret = m_default_ip;
	for (const auto &ip_array : m_external_ips) {
		if (ip_array.try_match_ip_to_subnet(test, ret)) {
			break;
		}
	}

	return ret;
}

}