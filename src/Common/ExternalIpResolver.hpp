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

#include "Common/ExternalIp.hpp"
#include "Common/Ip.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class ExternalIpResolver {
	public:
		ExternalIpResolver(const Ip &defaultIp, const IpMatrix &externalIps);
		ExternalIpResolver() : m_defaultIp{0} { }

		auto setExternalIpInformation(const Ip &defaultIp, const IpMatrix &matrix) -> void {
			m_defaultIp = defaultIp;
			m_externalIps = matrix;
		}

		auto matchIpToSubnet(const Ip &test) const -> Ip;
		auto getExternalIps() const -> const IpMatrix & { return m_externalIps; }
	protected:
		IpMatrix m_externalIps;
		Ip m_defaultIp;
	};
}