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

#include "Ip.h"
#include "IPacket.h"
#include "Types.h"
#include <string>
#include <vector>

class PacketCreator;
class PacketReader;

class ExternalIp : public Ip {
public:
	ExternalIp(const string_t &addr, uint32_t ipv4SubnetMask);
	ExternalIp(uint32_t ipv4, uint32_t ipv4SubnetMask);

	auto tryMatchIpToSubnet(const Ip &test, Ip &result) const -> bool;
	auto write(PacketCreator &packet) const -> void override;
	auto read(PacketReader &packet) -> void override;
private:
	friend class PacketReader;
	ExternalIp() : Ip() { }

	uint32_t m_ipv4SubnetMask = 0;
};

using IpMatrix = vector_t<ExternalIp>;