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

#include "common/enum_base.hpp"
#include "common/types.hpp"
#include <string>
#include <type_traits>

namespace vana {
	namespace util {
		namespace enum_cast {
			template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
			inline
			auto try_cast_from_underlying(TUnderlying value, TEnum &out) -> result {
				return enum_caster<TEnum, TUnderlying>::try_cast_from_underlying(value, out);
			}

			template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
			inline
			auto from_underlying(TUnderlying value) -> TEnum {
				return enum_caster<TEnum, TUnderlying>::cast_from_underlying(value);
			}

			template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
			inline
			auto try_cast_to_underlying(TEnum value, TUnderlying &out) -> result {
				return enum_caster<TEnum, TUnderlying>::try_cast_to_underlying(value, out);
			}

			template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
			inline
			auto to_underlying(TEnum value) -> TUnderlying {
				return enum_caster<TEnum, TUnderlying>::cast_to_underlying(value);
			}

			template <typename TEnum>
			inline
			auto to_string(TEnum value) -> std::string {
				return enum_stringifier<TEnum>{}.to_string(value);
			}

			template <typename TEnum>
			inline
			auto try_get_from_string(const std::string &value, TEnum &out) -> result {
				return enum_stringifier<TEnum>{}.try_get_from_string(value, out);
			}

			template <typename TEnum>
			inline
			auto get_from_string(const std::string &value) -> TEnum {
				return enum_stringifier<TEnum>{}.get_from_string(value);
			}
		}
	}
}