/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "hash_combine.hpp"
#include "TimerTypes.hpp"
#include "Types.hpp"
#include <algorithm>
#include <functional>

namespace Timer {

struct Id {
	NO_DEFAULT_CONSTRUCTOR(Id);
public:
	Id(uint32_t type, uint32_t id, uint32_t id2);
	uint32_t type;
	uint32_t id;
	uint32_t id2;

	auto operator==(const Id &other) const -> bool;
};

inline
Id::Id(uint32_t type, uint32_t id, uint32_t id2) :
	type(type),
	id(id),
	id2(id2)
{
}

inline
auto Id::operator==(const Id &other) const -> bool {
	return type == other.type && id == other.id && id2 == other.id2;
}

}

namespace std {
	template <>
	struct hash<Timer::Id> {
		auto operator()(const Timer::Id &v) const -> size_t {
			return MiscUtilities::hash_combinator(v.type, v.id, v.id2);
		}
	};
}