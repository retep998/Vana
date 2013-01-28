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

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace MiscUtilities {
	// Sorta based on Boost's, not really
	class tokenizer {
	public:
		tokenizer(const string &content, const string &sep) {
			size_t start = 0;
			size_t end = 0;
			while (end != string::npos) {
				end = content.find(sep, start);
				m_tokens.push_back(content.substr(start, (end == string::npos) ? end : end - start));
				start = (end > (string::npos - sep.size()) ? string::npos : end + sep.size());
			}
		}
		typedef vector<string>::iterator iterator;
		typedef vector<string>::const_iterator const_iterator;
		iterator begin() { return m_tokens.begin(); }
		iterator end() { return m_tokens.end(); }
		const_iterator cbegin() const { return m_tokens.cbegin(); }
		const_iterator cend() const { return m_tokens.cend(); }
	private:
		vector<string> m_tokens;
	};
}