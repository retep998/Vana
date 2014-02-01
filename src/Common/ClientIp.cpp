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
#include "ClientIp.hpp"
#include "PacketBuilder.hpp"
#include <boost/asio.hpp>
#include <stdexcept>

ClientIp::ClientIp(const Ip &ip) :
	m_ip(ip)
{
}

auto ClientIp::write(PacketBuilder &builder) const -> void {
	if (m_ip.getType() == Ip::Type::Ipv4) {
		builder.add<uint32_t>(htonl(m_ip.asIpv4()));
	}
	else {
		throw std::invalid_argument("IPv6 unsupported");
	}
}