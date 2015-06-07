/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ExternalIpResolver.hpp"
#include "Ip.hpp"
#include "Types.hpp"

class Channel : public ExternalIpResolver {
	NONCOPYABLE(Channel);
public:
	Channel() = default;
	auto setPort(port_t port) -> void { m_port = port; }
	auto setPopulation(int32_t population) -> void { m_population = population; }
	auto getPort() const -> port_t { return m_port; }
	auto getPopulation() const -> int32_t { return m_population; }
private:
	port_t m_port = 0;
	int32_t m_population = 0;
};