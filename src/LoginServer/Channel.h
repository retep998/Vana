/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "Ip.h"
#include "Types.h"

class Channel {
public:
	Channel() : m_population(0) { }

	void setPort(uint16_t port) { m_port = port; }
	void setPopulation(int32_t population) { m_population = population; }
	void setIp(uint32_t ip) { m_ip = ip; }
	void setExternalIps(const IpMatrix &matrix) { m_externalIps = matrix; }

	uint16_t getPort() const { return m_port; }
	int32_t getPopulation() const { return m_population; }
	uint32_t getIp() const { return m_ip; }
	IpMatrix & getExternalIps() { return m_externalIps; }
private:
	uint16_t m_port;
	int32_t m_population;
	uint32_t m_ip;
	IpMatrix m_externalIps;
};
