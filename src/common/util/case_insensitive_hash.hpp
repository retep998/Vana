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

#include <locale>
#include <string>

namespace vana {
	namespace util {
		struct case_insensitive_hash {
			auto operator()(const std::string &s) const -> size_t {
		#if SIZE_MAX == UINT32_MAX
				size_t const prime = 16777619U;
				size_t const base = 2166136261U;
		#elif SIZE_MAX == UINT64_MAX
				size_t const prime = 1099511628211ULL;
				size_t const base = 14695981039346656037ULL;
		#else
		#	error "Unsupported platform"
		#endif
				size_t val = base;
				std::locale loc;
				for (const auto &c : s) {
					val ^= static_cast<size_t>(std::tolower(c, loc));
					val *= prime;
				}
				return val;
			}
		};
	}
}