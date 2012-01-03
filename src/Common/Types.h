/*
Copyright (C) 2008-2012 Vana Development Team

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

/* Standard integers */
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <string>

// Import the type from the boost namespace
typedef boost::int8_t int8_t;
typedef boost::uint8_t uint8_t;
typedef boost::int16_t int16_t;
typedef boost::uint16_t uint16_t;
typedef boost::int32_t int32_t;
typedef boost::uint32_t uint32_t;
typedef boost::int64_t int64_t;
typedef boost::uint64_t uint64_t;

// Types for use with database
class unix_time_t {
public:
	unix_time_t() { m_time = time(nullptr); }
	unix_time_t(time_t t) { m_time = t; }
	unix_time_t(const unix_time_t &t) { m_time = t.m_time; }
	operator time_t() const { return m_time; }
	unix_time_t & operator =(const time_t &t) { m_time = t; return *this; }
	unix_time_t & operator =(const unix_time_t &right) { m_time = right.m_time; return *this; }
	unix_time_t & operator +=(const time_t &t) { m_time += t; return *this; }
	unix_time_t & operator +=(const unix_time_t &right) { m_time += right.m_time; return *this; }
	unix_time_t & operator -=(const time_t &t) { m_time -= t; return *this; }
	unix_time_t & operator -=(const unix_time_t &right) { m_time -= right.m_time; return *this; }
	unix_time_t operator +(const time_t &t) { return unix_time_t(m_time + t); }
	unix_time_t operator +(const unix_time_t &right) { return unix_time_t(m_time + right.m_time); }
	unix_time_t operator -(const time_t &t) { return unix_time_t(m_time - t); }
	unix_time_t operator -(const unix_time_t &right) { return unix_time_t(m_time - right.m_time); }
private:
	time_t m_time;
};

using boost::optional;
typedef optional<bool> opt_bool;
typedef optional<int8_t> opt_int8_t;
typedef optional<uint8_t> opt_uint8_t;
typedef optional<int16_t> opt_int16_t;
typedef optional<uint16_t> opt_uint16_t;
typedef optional<int32_t> opt_int32_t;
typedef optional<uint32_t> opt_uint32_t;
typedef optional<int64_t> opt_int64_t;
typedef optional<uint64_t> opt_uint64_t;
typedef optional<double> opt_double;
typedef optional<unix_time_t> opt_unix_time_t;
typedef optional<std::string> opt_string;


// Vana-specific types
typedef uint16_t header_t;
typedef uint16_t port_t;
typedef uint32_t ip_t;