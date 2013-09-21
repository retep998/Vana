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

#include "IPacket.h"
#include "Types.h"
#include <stdexcept>
#include <string>

using std::string;

class PacketCreator;
class PacketReader;

class Ip : public IPacketSerializable {
public:
	class Type : public IPacketSerializable {
	public:
		Type(int8_t ipType) : m_type(ipType)
		{
			if (ipType != Ipv4 && ipType != Ipv6) {
				throw std::invalid_argument("Must pass Ip::Type::Ipv4 or Ip::Type::Ipv6 to the constructor");
			}
		}
		void write(PacketCreator &packet) const override;
		void read(PacketReader &packet) override;

		bool operator==(const Type &right) const { return right.m_type == this->m_type; }
		bool operator==(const int8_t &right) const { return right == this->m_type; }
		bool operator!=(const Type &right) const { return right.m_type != this->m_type; }
		bool operator!=(const int8_t &right) const { return right != this->m_type; }

		static const int8_t Ipv4 = 1;
		static const int8_t Ipv6 = 2;
	private:
		friend class PacketReader;
		friend class Ip;
		Type() { }
		int8_t m_type;
	};

	Ip(const string &addr, Ip::Type type);
	explicit Ip(uint32_t ipv4);

	virtual void write(PacketCreator &packet) const override;
	virtual void read(PacketReader &packet) override;

	string toString() const;
	uint32_t asIpv4() const { return m_ipv4; }
	const Ip::Type & getType() const { return m_type; }
	bool isInitialized() const { return m_ipv4 != 0; }

	bool operator==(const Ip &right) const { return right.m_type == this->m_type && right.m_ipv4 == this->m_ipv4; }
	bool operator!=(const Ip &right) const { return right.m_type != this->m_type || right.m_ipv4 != this->m_ipv4; }

	static uint32_t stringToIpv4(const string &name);
protected:
	friend class PacketReader;
	friend class ExternalIp;
	Ip() { }

	uint32_t m_ipv4;
	Ip::Type m_type;
};