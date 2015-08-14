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

#include "hash_combine.hpp"
#include "Types.hpp"

// Thin wrapper around time_t so we could write a SOCI extension for it
// time_t on its own is defined as __int64_t on MSVC which was conflicting with the SOCI extension for int64_t
class UnixTime {
public:
	UnixTime() { m_time = time(nullptr); }
	UnixTime(time_t t) { m_time = t; }
	UnixTime(const UnixTime &t) { m_time = t.m_time; }
	operator time_t() const { return m_time; }
	auto operator =(const time_t &t) -> UnixTime & { m_time = t; return *this; }
	auto operator =(const UnixTime &right) -> UnixTime & { m_time = right.m_time; return *this; }
	auto operator +=(const time_t &t) -> UnixTime & { m_time += t; return *this; }
	auto operator +=(const UnixTime &right) -> UnixTime & { m_time += right.m_time; return *this; }
	auto operator -=(const time_t &t) -> UnixTime & { m_time -= t; return *this; }
	auto operator -=(const UnixTime &right) -> UnixTime & { m_time -= right.m_time; return *this; }
	auto operator +(const time_t &t) const -> UnixTime { return UnixTime{m_time + t}; }
	auto operator +(const UnixTime &right) const -> UnixTime { return UnixTime{m_time + right.m_time}; }
	auto operator -(const time_t &t) const -> UnixTime { return UnixTime{m_time - t}; }
	auto operator -(const UnixTime &right) const -> UnixTime { return UnixTime{m_time - right.m_time}; }
private:
	time_t m_time;

	friend auto operator ==(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator !=(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator <(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator <=(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator >(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator >=(const UnixTime &a, const UnixTime &b) -> bool;
	friend auto operator ==(time_t a, const UnixTime &b) -> bool;
	friend auto operator !=(time_t a, const UnixTime &b) -> bool;
	friend auto operator <(time_t a, const UnixTime &b) -> bool;
	friend auto operator <=(time_t a, const UnixTime &b) -> bool;
	friend auto operator >(time_t a, const UnixTime &b) -> bool;
	friend auto operator >=(time_t a, const UnixTime &b) -> bool;
	friend auto operator ==(const UnixTime &a, time_t b) -> bool;
	friend auto operator !=(const UnixTime &a, time_t b) -> bool;
	friend auto operator <(const UnixTime &a, time_t b) -> bool;
	friend auto operator <=(const UnixTime &a, time_t b) -> bool;
	friend auto operator >(const UnixTime &a, time_t b) -> bool;
	friend auto operator >=(const UnixTime &a, time_t b) -> bool;
};

inline
auto operator ==(const UnixTime &a, const UnixTime &b) -> bool {
	return a.m_time == b.m_time;
}

inline
auto operator !=(const UnixTime &a, const UnixTime &b) -> bool {
	return !(a == b);
}

inline
auto operator >(const UnixTime &a, const UnixTime &b) -> bool {
	return a.m_time > b.m_time;
}

inline
auto operator >=(const UnixTime &a, const UnixTime &b) -> bool {
	return a.m_time >= b.m_time;
}

inline
auto operator <(const UnixTime &a, const UnixTime &b) -> bool {
	return a.m_time < b.m_time;
}

inline
auto operator <=(const UnixTime &a, const UnixTime &b) -> bool {
	return a.m_time <= b.m_time;
}

inline
auto operator ==(time_t a, const UnixTime &b) -> bool {
	return a == b.m_time;
}

inline
auto operator !=(time_t a, const UnixTime &b) -> bool {
	return !(a == b);
}

inline
auto operator >(time_t a, const UnixTime &b) -> bool {
	return a > b.m_time;
}

inline
auto operator >=(time_t a, const UnixTime &b) -> bool {
	return a >= b.m_time;
}

inline
auto operator <(time_t a, const UnixTime &b) -> bool {
	return a < b.m_time;
}

inline
auto operator <=(time_t a, const UnixTime &b) -> bool {
	return a <= b.m_time;
}

inline
auto operator ==(const UnixTime &a, time_t b) -> bool {
	return a.m_time == b;
}

inline
auto operator !=(const UnixTime &a, time_t b) -> bool {
	return !(a == b);
}

inline
auto operator >(const UnixTime &a, time_t b) -> bool {
	return a.m_time > b;
}

inline
auto operator >=(const UnixTime &a, time_t b) -> bool {
	return a.m_time >= b;
}

inline
auto operator <(const UnixTime &a, time_t b) -> bool {
	return a.m_time < b;
}

inline
auto operator <=(const UnixTime &a, time_t b) -> bool {
	return a.m_time <= b;
}

namespace std {

template <>
struct hash<UnixTime> {
	auto operator()(const UnixTime &v) const -> size_t {
		return MiscUtilities::hash_combinator(static_cast<time_t>(v));
	}
};

}