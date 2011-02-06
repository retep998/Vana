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
#include "StringUtilities.h"
#include <boost/tokenizer.hpp>

int32_t StringUtilities::noCaseCompare(const string &s1, const string &s2) {
	string::const_iterator iter1 = s1.begin();
	string::const_iterator iter2 = s2.begin();

	while ((iter1 != s1.end()) && (iter2 != s2.end())) { 
		if (toupper(*iter1) != toupper(*iter2)) {
			return (toupper(*iter1) < toupper(*iter2)) ? -1 : 1; 
		}
		iter1++;
		iter2++;
	}

	// The letters are the same, so let's return based on size
	size_t l1 = s1.size(), l2 = s2.size();
	if (l1 == l2) {
		return 0;
	}
	else {
		return (l1 < l2) ? -1 : 1;
	}
}

void StringUtilities::runFlags(const string &flags, function<void (string)> func) {
	if (flags.length() > 0) {
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		typedef boost::char_separator<char> separator;

		separator sep(",");
		tokenizer tokens(flags, sep);

		for (tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); iter++) {
			func(*iter);
		}
	}
}

int64_t StringUtilities::atoli(char *str) {
	int64_t result = 0;
	while (*str >= '0' && *str <= '9') {
		result = (result * 10) + (*str++ - '0');
	}
	return result;
}