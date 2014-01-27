/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "ExternalIpResolver.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include <stdexcept>

ExternalIpResolver::ExternalIpResolver(const Ip &defaultIp, const IpMatrix &externalIps) :
	m_defaultIp(defaultIp),
	m_externalIps(externalIps)
{
}

auto ExternalIpResolver::matchIpToSubnet(const Ip &test) const -> Ip {
	if (test.getType() != m_defaultIp.getType()) throw std::invalid_argument("IP type must match the external IP type");

	Ip ret = m_defaultIp;
	for (const auto &ipArray : m_externalIps) {
		if (ipArray.tryMatchIpToSubnet(test, ret)) {
			break;
		}
	}

	return ret;
}