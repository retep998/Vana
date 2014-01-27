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
#include "PacketCreator.h"
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
	return "";
}

auto Ip::write(PacketCreator &packet) const -> void {
	packet.addClass<Ip::Type>(m_type);
	if (m_type == Ip::Type::Ipv4) {
		packet.add<uint32_t>(m_ipv4);
	}
}

auto Ip::read(PacketReader &packet) -> void {
	m_type = packet.getClass<Ip::Type>();
	if (m_type == Ip::Type::Ipv4) {
		m_ipv4 = packet.get<uint32_t>();
	}
}

auto Ip::Type::write(PacketCreator &packet) const -> void {
	packet.add<int8_t>(m_type);
}

auto Ip::Type::read(PacketReader &packet) -> void {
	m_type = packet.get<int8_t>();
}