/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/IPacket.hpp"
#include "Common/Types.hpp"
#include <stdexcept>
#include <string>

namespace vana {
	class packet_builder;
	class packet_reader;

	class ip {
	public:
		class type {
		public:
			type(int8_t ip_type) :
				m_type{ip_type}
			{
				if (ip_type != ipv4 && ip_type != ipv6) {
					throw std::invalid_argument{"Must pass ip::type::ipv4 or ip::type::ipv6 to the constructor"};
				}
			}

			auto operator==(const type &right) const -> bool { return right.m_type == this->m_type; }
			auto operator==(const int8_t &right) const -> bool { return right == this->m_type; }
			auto operator!=(const type &right) const -> bool { return right.m_type != this->m_type; }
			auto operator!=(const int8_t &right) const -> bool { return right != this->m_type; }

			static const int8_t ipv4 = 1;
			static const int8_t ipv6 = 2;
		private:
			friend class ip;
			friend struct packet_serialize<ip::type>;
			type() = default;
			int8_t m_type = -1;
		};

		ip(const string &addr, ip::type type);
		explicit ip(uint32_t ipv4);

		auto to_string() const -> string;
		auto as_ipv4() const -> uint32_t;
		auto get_type() const -> const ip::type &;
		auto is_initialized() const -> bool;

		auto operator==(const ip &right) const -> bool { return right.m_type == this->m_type && right.m_ipv4 == this->m_ipv4; }
		auto operator!=(const ip &right) const -> bool { return right.m_type != this->m_type || right.m_ipv4 != this->m_ipv4; }
		friend auto operator <<(std::ostream &out, const ip &ip) -> std::ostream &;

		static auto string_to_ipv4(const string &name) -> uint32_t;
	protected:
		friend class external_ip;
		friend struct packet_serialize<ip>;
		ip() = default;

		uint32_t m_ipv4 = 0;
		ip::type m_type = ip::type::ipv4;
	};

	template <>
	struct packet_serialize<ip::type> {
		auto read(packet_reader &reader) -> ip::type {
			ip::type ret(reader.get<int8_t>());
			return ret;
		}
		auto write(packet_builder &builder, const ip::type &obj) -> void {
			builder.add<int8_t>(obj.m_type);
		}
	};

	template <>
	struct packet_serialize<ip> {
		auto read(packet_reader &reader) -> ip {
			ip ret;
			ret.m_type = reader.get<ip::type>();
			if (ret.m_type == ip::type::ipv4) {
				ret.m_ipv4 = reader.get<uint32_t>();
			}
			return ret;
		}
		auto write(packet_builder &builder, const ip &obj) -> void {
			builder.add<ip::type>(obj.get_type());
			if (obj.get_type() == ip::type::ipv4) {
				builder.add<uint32_t>(obj.as_ipv4());
			}
		}
	};

	inline
	auto operator <<(std::ostream &out, const ip &ip) -> std::ostream & {
		return out << ip.to_string();
	}

	inline
	auto operator <<(std::ostream &out, const optional<ip> &ip) -> std::ostream & {
		if (ip.is_initialized()) return out << ip.get();
		return out << "{invalid ip}";
	}
}