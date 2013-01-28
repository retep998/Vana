/*
Copyright (C) 2008-2013 Vana Development Team

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

Ip ExternalIpResolver::matchIpToSubnet(const Ip &test) const {
	if (test.getType() != m_defaultIp.getType()) throw std::invalid_argument("IP type must match the external IP type");

	Ip ret = m_defaultIp;
	for (IpMatrix::const_iterator iter = m_externalIps.begin(); iter != m_externalIps.end(); ++iter) {
		const ExternalIp &ipArray = *iter;
		if (ipArray.tryMatchIpToSubnet(test, ret)) {
			break;
		}
	}

	return ret;
}