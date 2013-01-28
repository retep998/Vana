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

#include "ExternalIp.h"
#include "ExternalIpResolver.h"
#include "Ip.h"
#include "Types.h"

class Channel : public ExternalIpResolver {
public:
	Channel() : m_population(0) { }

	void setPort(port_t port) { m_port = port; }
	void setPopulation(int32_t population) { m_population = population; }
	port_t getPort() const { return m_port; }
	int32_t getPopulation() const { return m_population; }
private:
	port_t m_port;
	int32_t m_population;
};