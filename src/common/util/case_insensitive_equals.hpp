/*
Copyright (C) 2008-2016 Vana Development Team

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

#include <functional>
#include <string>

namespace vana {
	namespace util {
		struct case_insensitive_equals : public std::binary_function<std::string, std::string, bool> {
			auto operator()(const std::string &left, const std::string &right) const -> bool {
				if (left.size() != right.size()) return false;

				auto iter1 = std::begin(left);
				auto iter2 = std::begin(right);
				auto end1 = std::end(left);
				auto end2 = std::end(right);

				while ((iter1 != end1) && (iter2 != end2)) {
					if (toupper(*iter1) != toupper(*iter2)) {
						return false;
					}
					++iter1;
					++iter2;
				}

				return true;
			}
		};
	}
}