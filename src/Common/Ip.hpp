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

#include "IPacket.hpp"
#include "Types.hpp"
#include <stdexcept>
#include <string>

class PacketBuilder;
class PacketReader;

class Ip : public IPacketSerializable {
public:
	class Type : public IPacketSerializable {
	public:
		Type(int8_t ipType) :
			m_type(ipType)
		{
			if (ipType != Ipv4 && ipType != Ipv6) {
				throw std::invalid_argument("Must pass Ip::Type::Ipv4 or Ip::Type::Ipv6 to the constructor");
			}
		}
		auto write(PacketBuilder &builder) const -> void override;
		auto read(PacketReader &reader) -> void override;

		auto operator==(const Type &right) const -> bool { return right.m_type == this->m_type; }
		auto operator==(const int8_t &right) const -> bool { return right == this->m_type; }
		auto operator!=(const Type &right) const -> bool { return right.m_type != this->m_type; }
		auto operator!=(const int8_t &right) const -> bool { return right != this->m_type; }

		static const int8_t Ipv4 = 1;
		static const int8_t Ipv6 = 2;
	private:
		friend class PacketReader;
		friend class Ip;
		Type() = default;
		int8_t m_type = -1;
	};

	Ip(const string_t &addr, Ip::Type type);
	explicit Ip(uint32_t ipv4);

	virtual auto write(PacketBuilder &builder) const -> void override;
	virtual auto read(PacketReader &reader) -> void override;

	auto toString() const -> string_t;
	auto asIpv4() const -> uint32_t;
	auto getType() const -> const Ip::Type &;
	auto isInitialized() const -> bool;

	auto operator==(const Ip &right) const -> bool { return right.m_type == this->m_type && right.m_ipv4 == this->m_ipv4; }
	auto operator!=(const Ip &right) const -> bool { return right.m_type != this->m_type || right.m_ipv4 != this->m_ipv4; }
	friend auto operator <<(std::ostream &out, const Ip &ip) -> std::ostream &;

	static auto stringToIpv4(const string_t &name) -> uint32_t;
protected:
	friend class PacketReader;
	friend class ExternalIp;
	Ip() = default;

	uint32_t m_ipv4 = 0;
	Ip::Type m_type = Ip::Type::Ipv4;
};

inline
auto operator <<(std::ostream &out, const Ip &ip) -> std::ostream & {
	return out << ip.toString();
}