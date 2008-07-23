/*
Copyright (C) 2008 Vana Development Team

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
#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

#include <sstream>
#include <string>

using std::string;

namespace StringUtilities {
	int noCaseCompare(const string &s1, const string &s2);

	template <typename T>
	T toType(const string &s);
}

template <typename T>
inline T StringUtilities::toType(const string &s) {
	std::istringstream i(s);
	T x;
	i >> x;
	return x;
} 

#endif
