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

#include "common/timer/type.hpp"
#include "common/types.hpp"
#include "common/util/hash_combine.hpp"
#include <algorithm>
#include <functional>

namespace vana {
	namespace timer {
		struct id {
			NO_DEFAULT_CONSTRUCTOR(id);
		public:
			id(vana::timer::type type, uint32_t id, uint32_t id2);
			id(vana::timer::type type, uint32_t id, int32_t id2);
			id(vana::timer::type type, uint32_t id);
			id(vana::timer::type type, int32_t id, uint32_t id2);
			id(vana::timer::type type, int32_t id, int32_t id2);
			id(vana::timer::type type, int32_t id);
			id(vana::timer::type type);

			uint32_t type;
			uint32_t id1;
			uint32_t id2;

			auto operator==(const id &other) const -> bool;
		};

		inline
		id::id(vana::timer::type type, uint32_t id, uint32_t id2) :
			type{static_cast<uint32_t>(type)},
			id1{id},
			id2{id2}
		{
		}

		inline
		id::id(vana::timer::type type, uint32_t id, int32_t id2) :
			type{static_cast<uint32_t>(type)},
			id1{id},
			id2{static_cast<uint32_t>(id2)}
		{
		}

		inline
		id::id(vana::timer::type type, uint32_t id) :
			type{static_cast<uint32_t>(type)},
			id1{id},
			id2{0}
		{
		}

		inline
		id::id(vana::timer::type type, int32_t id, uint32_t id2) :
			type{static_cast<uint32_t>(type)},
			id1{static_cast<uint32_t>(id)},
			id2{id2}
		{
		}

		inline
		id::id(vana::timer::type type, int32_t id, int32_t id2) :
			type{static_cast<uint32_t>(type)},
			id1{static_cast<uint32_t>(id)},
			id2{static_cast<uint32_t>(id2)}
		{
		}

		inline
		id::id(vana::timer::type type, int32_t id) :
			type{static_cast<uint32_t>(type)},
			id1{static_cast<uint32_t>(id)},
			id2{0}
		{
		}

		inline
		id::id(vana::timer::type type) :
			type{static_cast<uint32_t>(type)},
			id1{0},
			id2{0}
		{
		}

		inline
		auto id::operator==(const id &other) const -> bool {
			return type == other.type && id1 == other.id1 && id2 == other.id2;
		}
	}
}

namespace std {
	template <>
	struct hash<vana::timer::id> {
		auto operator()(const vana::timer::id &v) const -> size_t {
			return vana::util::hash_combinator(v.type, v.id1, v.id2);
		}
	};
}