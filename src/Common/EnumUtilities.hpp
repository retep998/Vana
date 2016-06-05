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

#include "Common/EnumBase.hpp"
#include "Common/Types.hpp"
#include <string>
#include <type_traits>

namespace vana {
	namespace enum_utilities {
		template <typename t_enum, typename t_underlying = typename std::underlying_type<t_enum>::type>
		inline
		auto try_cast_from_underlying(t_underlying value, t_enum &out) -> result {
			return enum_caster<t_enum, t_underlying>::try_cast_from_underlying(value, out);
		}

		template <typename t_enum, typename t_underlying = typename std::underlying_type<t_enum>::type>
		inline
		auto cast_from_underlying(t_underlying value) -> t_enum {
			return enum_caster<t_enum, t_underlying>::cast_from_underlying(value);
		}

		template <typename t_enum, typename t_underlying = typename std::underlying_type<t_enum>::type>
		inline
		auto try_cast_to_underlying(t_enum value, t_underlying &out) -> result {
			return enum_caster<t_enum, t_underlying>::try_cast_to_underlying(value, out);
		}

		template <typename t_enum, typename t_underlying = typename std::underlying_type<t_enum>::type>
		inline
		auto cast_to_underlying(t_enum value) -> t_underlying {
			return enum_caster<t_enum, t_underlying>::cast_to_underlying(value);
		}

		template <typename t_enum>
		inline
		auto to_string(t_enum value) -> std::string {
			return enum_stringifier<t_enum>{}.to_string(value);
		}

		template <typename t_enum>
		inline
		auto try_get_from_string(const std::string &value, t_enum &out) -> result {
			return enum_stringifier<t_enum>{}.try_get_from_string(value, out);
		}

		template <typename t_enum>
		inline
		auto get_from_string(const std::string &value) -> t_enum {
			return enum_stringifier<t_enum>{}.get_from_string(value);
		}
	}
}