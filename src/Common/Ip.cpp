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
#include "Ip.hpp"
#include "PacketBuilder.hpp"
#include <boost/asio.hpp>
#include <stdexcept>

Ip::Ip(const string_t &addr, Ip::Type type) :
	m_type(type)
{
	if (m_type != Ip::Type::Ipv4) {
		throw std::invalid_argument("Can only resolve Ip::Type::Ipv4");
	}
	m_ipv4 = stringToIpv4(addr);
}

Ip::Ip(uint32_t ipv4) :
	m_type(Ip::Type::Ipv4),
	m_ipv4(ipv4)
{
}

auto Ip::stringToIpv4(const string_t &name) -> uint32_t {
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::resolver resolver(ioService);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), name, "http");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	boost::asio::ip::tcp::resolver::iterator end;

	// boost::asio throws an exception if the name cannot be resolved

	boost::asio::ip::tcp::endpoint ep = *iter;
	return ep.address().to_v4().to_ulong();
}

auto Ip::toString() const -> string_t {
	if (m_type == Ip::Type::Ipv4) {
		return boost::asio::ip::address_v4(m_ipv4).to_string();
	}
	throw std::invalid_argument("m_type");
}

auto Ip::asIpv4() const -> uint32_t {
	return m_ipv4;
}

auto Ip::getType() const -> const Ip::Type & {
	return m_type;
}

auto Ip::isInitialized() const -> bool {
	return m_ipv4 != 0;
}