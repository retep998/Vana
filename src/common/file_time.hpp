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

#include "common/i_packet.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include "common/util/hash_combine.hpp"

namespace vana {
	class unix_time;

	class file_time {
	public:
		file_time();
		file_time(int64_t t);
		file_time(const unix_time &t);
		file_time(const file_time &t);

		auto get_value() const -> int64_t;
		auto operator =(const file_time &right) -> file_time & { m_value = right.m_value; return *this; }
		auto operator +=(const file_time &right) -> file_time & { m_value += right.m_value; return *this; }
		auto operator -=(const file_time &right) -> file_time & { m_value -= right.m_value; return *this; }
		auto operator +(const file_time &right) const -> file_time { return file_time{m_value + right.m_value}; }
		auto operator -(const file_time &right) const -> file_time { return file_time{m_value - right.m_value}; }
	private:
		int64_t m_value;

		static auto convert(time_t time) -> int64_t;
		friend auto operator ==(const file_time &a, const file_time &b) -> bool;
		friend auto operator !=(const file_time &a, const file_time &b) -> bool;
		friend auto operator <(const file_time &a, const file_time &b) -> bool;
		friend auto operator <=(const file_time &a, const file_time &b) -> bool;
		friend auto operator >(const file_time &a, const file_time &b) -> bool;
		friend auto operator >=(const file_time &a, const file_time &b) -> bool;
	};

	inline
	auto operator ==(const file_time &a, const file_time &b) -> bool {
		return a.m_value == b.m_value;
	}

	inline
	auto operator !=(const file_time &a, const file_time &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const file_time &a, const file_time &b) -> bool {
		return a.m_value > b.m_value;
	}

	inline
	auto operator >=(const file_time &a, const file_time &b) -> bool {
		return a.m_value >= b.m_value;
	}

	inline
	auto operator <(const file_time &a, const file_time &b) -> bool {
		return a.m_value < b.m_value;
	}

	inline
	auto operator <=(const file_time &a, const file_time &b) -> bool {
		return a.m_value <= b.m_value;
	}

	template <>
	struct packet_serialize<file_time> {
		auto read(packet_reader &reader) -> file_time {
			return file_time{reader.get<int64_t>()};
		}
		auto write(packet_builder &builder, const file_time &obj) -> void {
			builder.add<int64_t>(obj.get_value());
		}
	};
}

namespace std {
	template <>
	struct hash<vana::file_time> {
		auto operator()(const vana::file_time &v) const -> size_t {
			return vana::util::hash_combinator(static_cast<int64_t>(v.get_value()));
		}
	};
}