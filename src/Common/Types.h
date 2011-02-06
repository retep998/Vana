/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef TYPES_H
#define TYPES_H

/* Standard integers */
#include <boost/cstdint.hpp>

// Import the type from the boost namespace
typedef boost::int8_t int8_t;
typedef boost::uint8_t uint8_t;
typedef boost::int16_t int16_t;
typedef boost::uint16_t uint16_t;
typedef boost::int32_t int32_t;
typedef boost::uint32_t uint32_t;
typedef boost::int64_t int64_t;
typedef boost::uint64_t uint64_t;
typedef boost::intmax_t intmax_t;
typedef boost::uintmax_t uintmax_t;

#endif