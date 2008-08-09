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
#include "StringUtilities.h"
#include <algorithm>

int StringUtilities::noCaseCompare(const string &s1, const string &s2) {
	string::const_iterator iter1 = s1.begin();
	string::const_iterator iter2 = s2.begin();

	while ((iter1 != s1.end()) && (iter2 != s2.end())) { 
		if (toupper(*iter1) != toupper(*iter2)) {
			return (toupper(*iter1)  < toupper(*iter2)) ? -1 : 1; 
		}
		iter1++;
		iter2++;
	}

	// The letters are the same, so lets return based on size
	size_t l1 = s1.size(), l2 = s2.size();
	if (l1 == l2) {
		return 0;
	}
	else {
		return (l1 < l2) ? -1 : 1;
	}
}

string StringUtilities::toUpper(const string &s) {
	string upper = s;
	std::transform(upper.begin(), upper.end(), upper.begin(), toupper);
	return upper;
}

void StringUtilities::trim(string &s) {
	s.erase(s.find_last_not_of(" ") + 1);
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of("\r") + 1);
	s.erase(0, s.find_first_not_of("\r"));
	s.erase(s.find_last_not_of("\n") + 1);
	s.erase(0, s.find_first_not_of("\n"));
}

string StringUtilities::trim(const string &s) {
	string r = s;
	trim(r);
	return r;
}
