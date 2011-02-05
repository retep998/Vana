/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the #implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

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

typedef int16_t header_t; // Allows for easier transitioning when the header type isn't 2 bytes

// Take out this ugliness when C++0x is widely supported, find other instances of "C++0x" and remove
#include <boost/config.hpp>
#if _MSC_VER < 1600
// VS2010 (version 1600) supports nullptr, Boost 1.41 doesn't have this info
# define nullptr 0
// Yes, this is evil, deal with it
#elif defined BOOST_NO_NULLPTR && _MSC_VER != 1600
# define nullptr 0
#endif

namespace ServerTypes {
	enum ServerTypes {
		Login = 1, // Started at 1 so it's easy to do logging
		World,
		Channel,
		Cash,
		Mts
	};
}