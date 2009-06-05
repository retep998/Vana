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
#ifndef MISCUTILITIES
#define MISCUTILITIES

#include <cstdlib>
#include <string>

using std::string;

namespace MiscUtilities {
	bool atob(const char *str);
	string hashPassword(const string &password, const string &salt);

	// The following methods are used for deleting (freeing) pointers in an array
	template<class T>
	struct DeleterSeq {
		void operator()(T *t) { delete t; }
	};

	template<class T>
	struct DeleterPairAssoc {
		void operator()(T pair) { delete pair.second; } 
	};
};

inline
bool MiscUtilities::atob(const char *str) {
	return atoi(str) != 0;
}

#endif
