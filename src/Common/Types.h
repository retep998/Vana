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

#include "optional.hpp"
#include <algorithm>
#include <cstdint>
#include <string>

// Thin wrapper around time_t so we could write a SOCI extension for it
// time_t on its own is defined as __int64_t on MSVC which was conflicting with the SOCI extension for int64_t
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

// Useful DB-related aliases
typedef MiscUtilities::optional<bool> opt_bool;
typedef MiscUtilities::optional<int8_t> opt_int8_t;
typedef MiscUtilities::optional<uint8_t> opt_uint8_t;
typedef MiscUtilities::optional<int16_t> opt_int16_t;
typedef MiscUtilities::optional<uint16_t> opt_uint16_t;
typedef MiscUtilities::optional<int32_t> opt_int32_t;
typedef MiscUtilities::optional<uint32_t> opt_uint32_t;
typedef MiscUtilities::optional<int64_t> opt_int64_t;
typedef MiscUtilities::optional<uint64_t> opt_uint64_t;
typedef MiscUtilities::optional<double> opt_double;
typedef MiscUtilities::optional<unix_time_t> opt_unix_time_t;
typedef MiscUtilities::optional<std::string> opt_string;

// Miscellaneous utility types
typedef uint16_t header_t;
typedef uint16_t port_t;

// Remove when VS2012 supports thread_local
#ifdef WIN32
#define thread_local __declspec(thread)
#else
#define thread_local __thread
#endif