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

#include "common/external_ip.hpp"
#include "common/ip_temp.hpp"
#include "common/types_temp.hpp"

namespace vana {
	class external_ip_resolver {
	public:
		external_ip_resolver(const ip &default_ip, const ip_matrix &external_ips);
		external_ip_resolver() : m_default_ip{0} { }

		auto set_external_ip_information(const ip &default_ip, const ip_matrix &matrix) -> void {
			m_default_ip = default_ip;
			m_external_ips = matrix;
		}

		auto match_ip_to_subnet(const ip &test) const -> ip;
		auto get_external_ips() const -> const ip_matrix & { return m_external_ips; }
	protected:
		ip_matrix m_external_ips;
		ip m_default_ip;
	};
}