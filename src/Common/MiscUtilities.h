/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "Types.h"
#include <cstdlib>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace MiscUtilities {
	string hashPassword(const string &password, const string &salt);
	bool isBossChannel(const vector<int8_t> &vec, int8_t channelid);
	string getBanReason(int8_t reason);

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
