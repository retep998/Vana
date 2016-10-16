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

#include "common/types.hpp"
#include "common/util/hash_combine.hpp"

namespace vana {
	// Thin wrapper around time_t so we could write a SOCI extension for it
	// time_t on its own is defined as __int64_t on MSVC which was conflicting with the SOCI extension for int64_t
	class unix_time {
	public:
		unix_time() { m_time = time(nullptr); }
		unix_time(time_t t) { m_time = t; }
		unix_time(const unix_time &t) { m_time = t.m_time; }
		operator time_t() const { return m_time; }
		auto operator =(const time_t &t) -> unix_time & { m_time = t; return *this; }
		auto operator =(const unix_time &right) -> unix_time & { m_time = right.m_time; return *this; }
		auto operator +=(const time_t &t) -> unix_time & { m_time += t; return *this; }
		auto operator +=(const unix_time &right) -> unix_time & { m_time += right.m_time; return *this; }
		auto operator -=(const time_t &t) -> unix_time & { m_time -= t; return *this; }
		auto operator -=(const unix_time &right) -> unix_time & { m_time -= right.m_time; return *this; }
		auto operator +(const time_t &t) const -> unix_time { return unix_time{m_time + t}; }
		auto operator +(const unix_time &right) const -> unix_time { return unix_time{m_time + right.m_time}; }
		auto operator -(const time_t &t) const -> unix_time { return unix_time{m_time - t}; }
		auto operator -(const unix_time &right) const -> unix_time { return unix_time{m_time - right.m_time}; }
	private:
		time_t m_time;

		friend auto operator ==(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator !=(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator <(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator <=(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator >(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator >=(const unix_time &a, const unix_time &b) -> bool;
		friend auto operator ==(time_t a, const unix_time &b) -> bool;
		friend auto operator !=(time_t a, const unix_time &b) -> bool;
		friend auto operator <(time_t a, const unix_time &b) -> bool;
		friend auto operator <=(time_t a, const unix_time &b) -> bool;
		friend auto operator >(time_t a, const unix_time &b) -> bool;
		friend auto operator >=(time_t a, const unix_time &b) -> bool;
		friend auto operator ==(const unix_time &a, time_t b) -> bool;
		friend auto operator !=(const unix_time &a, time_t b) -> bool;
		friend auto operator <(const unix_time &a, time_t b) -> bool;
		friend auto operator <=(const unix_time &a, time_t b) -> bool;
		friend auto operator >(const unix_time &a, time_t b) -> bool;
		friend auto operator >=(const unix_time &a, time_t b) -> bool;
	};

	inline
	auto operator ==(const unix_time &a, const unix_time &b) -> bool {
		return a.m_time == b.m_time;
	}

	inline
	auto operator !=(const unix_time &a, const unix_time &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const unix_time &a, const unix_time &b) -> bool {
		return a.m_time > b.m_time;
	}

	inline
	auto operator >=(const unix_time &a, const unix_time &b) -> bool {
		return a.m_time >= b.m_time;
	}

	inline
	auto operator <(const unix_time &a, const unix_time &b) -> bool {
		return a.m_time < b.m_time;
	}

	inline
	auto operator <=(const unix_time &a, const unix_time &b) -> bool {
		return a.m_time <= b.m_time;
	}

	inline
	auto operator ==(time_t a, const unix_time &b) -> bool {
		return a == b.m_time;
	}

	inline
	auto operator !=(time_t a, const unix_time &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(time_t a, const unix_time &b) -> bool {
		return a > b.m_time;
	}

	inline
	auto operator >=(time_t a, const unix_time &b) -> bool {
		return a >= b.m_time;
	}

	inline
	auto operator <(time_t a, const unix_time &b) -> bool {
		return a < b.m_time;
	}

	inline
	auto operator <=(time_t a, const unix_time &b) -> bool {
		return a <= b.m_time;
	}

	inline
	auto operator ==(const unix_time &a, time_t b) -> bool {
		return a.m_time == b;
	}

	inline
	auto operator !=(const unix_time &a, time_t b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const unix_time &a, time_t b) -> bool {
		return a.m_time > b;
	}

	inline
	auto operator >=(const unix_time &a, time_t b) -> bool {
		return a.m_time >= b;
	}

	inline
	auto operator <(const unix_time &a, time_t b) -> bool {
		return a.m_time < b;
	}

	inline
	auto operator <=(const unix_time &a, time_t b) -> bool {
		return a.m_time <= b;
	}
}

namespace std {
	template <>
	struct hash<vana::unix_time> {
		auto operator()(const vana::unix_time &v) const -> size_t {
			return vana::util::hash_combinator(static_cast<time_t>(v));
		}
	};
}