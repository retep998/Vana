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

#include "common/preprocessor.hpp"
#include "common/types.hpp"
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace vana {
	DEFAULT_EXCEPTION(invalid_enum_exception, std::runtime_error);

	template <typename TEnum, typename TUnderlying>
	struct enum_caster {
		static auto try_cast_from_underlying(TUnderlying value, TEnum &out) -> result {
			static_assert(false, "try_cast_from_underlying is not appropriately specialized for those types");
			throw std::logic_error{"try_cast_from_underlying is not appropriately specialized for those types"};
		}
		static auto cast_from_underlying(TUnderlying value) -> TEnum {
			static_assert(false, "cast_from_underlying is not appropriately specialized for those types");
			throw std::logic_error{"cast_from_underlying is not appropriately specialized for those types"};
		}
		static auto try_cast_to_underlying(TEnum value, TUnderlying &out) -> result {
			static_assert(false, "try_cast_to_underlying is not appropriately specialized for those types");
			throw std::logic_error{"try_cast_to_underlying is not appropriately specialized for those types"};
		}
		static auto cast_to_underlying(TEnum value) -> TUnderlying {
			static_assert(false, "cast_to_underlying is not appropriately specialized for those types");
			throw std::logic_error{"cast_to_underlying is not appropriately specialized for those types"};
		}
	};

	template <typename TEnum>
	struct enum_stringifier {
		auto to_string(TEnum value) -> std::string {
			static_assert(false, "to_string is not appropriately specialized for that type");
			throw std::logic_error{"to_string is not appropriately specialized for that type"};
		}
		auto try_get_from_string(const std::string &value, TEnum &out) -> result {
			static_assert(false, "tryCastFromString is not appropriately specialized for that type");
			throw std::logic_error{"tryCastFromString is not appropriately specialized for that type"};
		}
		auto get_from_string(const std::string &value) -> TEnum {
			static_assert(false, "castFromString is not appropriately specialized for that type");
			throw std::logic_error{"castFromString is not appropriately specialized for that type"};
		}
	};
}

#define ENUM_CLASS_VALUE_IMPL_2(TypeName, ValueName) \
	ValueName,

#define ENUM_CLASS_VALUE_IMPL_3(TypeName, ValueName, Value) \
	ValueName = Value,

#define ENUM_CLASS_VALUE_IMPL(...) \
	DISPATCH(ENUM_CLASS_VALUE_IMPL_, __VA_ARGS__)

#define CASE_VALUE_IMPL_2(TypeName, ValueName) \
	case TypeName::ValueName:

#define CASE_VALUE_IMPL_3(TypeName, ValueName, Value) \
	case TypeName::ValueName:

#define CASE_VALUE_IMPL(...) \
	DISPATCH(CASE_VALUE_IMPL_, __VA_ARGS__)

#define CAST_STRING_IMPL_2(TypeName, ValueName) \
	std::make_tuple<TypeName, std::string>(TypeName::ValueName, #ValueName),

#define CAST_STRING_IMPL_3(TypeName, ValueName, Value) \
	std::make_tuple<TypeName, std::string>(TypeName::ValueName, #ValueName),

#define CAST_STRING_IMPL(...) \
	DISPATCH(CAST_STRING_IMPL_, __VA_ARGS__)

#define ENUM_CLASS_IMPL_3(TypeName, UnderlyingType, List) \
	enum class TypeName : UnderlyingType { \
		DEFER(List)(TypeName, ENUM_CLASS_VALUE_IMPL) \
	}; \

#define ENUM_CLASS_IMPL_2(TypeName, List) \
	ENUM_CLASS_IMPL_3(TypeName, int, List)

#define ENUM_CLASS(...) \
	DISPATCH(ENUM_CLASS_IMPL_, __VA_ARGS__)

#define ENUM_CLASS_CAST_IMPL_3(FullyQualifiedTypeName, UnderlyingType, List) \
	namespace vana { \
		template <> \
		struct enum_caster<FullyQualifiedTypeName, UnderlyingType> { \
			static auto try_cast_from_underlying(UnderlyingType value, FullyQualifiedTypeName &out) -> result { \
				out = static_cast<FullyQualifiedTypeName>(value); \
				switch (out) { \
					DEFER(List)(FullyQualifiedTypeName, CASE_VALUE_IMPL) \
						return result::success; \
					default: \
						return result::failure; \
				} \
			} \
			static auto cast_from_underlying(UnderlyingType value) -> FullyQualifiedTypeName { \
				FullyQualifiedTypeName ret; \
				if (try_cast_from_underlying(value, ret) != result::success) { \
					throw invalid_enum_exception{#FullyQualifiedTypeName}; \
				} \
				return ret; \
			} \
			static auto try_cast_to_underlying(FullyQualifiedTypeName value, UnderlyingType &out) -> result { \
				out = static_cast<UnderlyingType>(value); \
				return result::success; \
			} \
			static auto cast_to_underlying(FullyQualifiedTypeName value) -> UnderlyingType { \
				UnderlyingType ret; \
				if (try_cast_to_underlying(value, ret) != result::success) { \
					throw invalid_enum_exception{#FullyQualifiedTypeName}; \
				} \
				return ret; \
			} \
		}; \
		template <> \
		struct enum_stringifier<FullyQualifiedTypeName> { \
		private: \
			std::vector<std::tuple<FullyQualifiedTypeName, std::string>> s_strings; \
		public: \
			enum_stringifier() { \
				s_strings = { \
					DEFER(List)(FullyQualifiedTypeName, CAST_STRING_IMPL) \
				}; \
			} \
			auto to_string(FullyQualifiedTypeName value) -> std::string { \
				auto iter = std::find_if( \
					std::begin(s_strings), \
					std::end(s_strings), \
					[&value](const std::tuple<FullyQualifiedTypeName, std::string> &test) -> bool { \
						return std::get<0>(test) == value; \
					}); \
				if (iter == std::end(s_strings)) return ""; \
				return std::get<1>(*iter); \
			} \
			auto try_get_from_string(const std::string &value, FullyQualifiedTypeName &out) -> result { \
				auto iter = std::find_if( \
					std::begin(s_strings), \
					std::end(s_strings), \
					[&value](const std::tuple<FullyQualifiedTypeName, std::string> &test) -> bool { \
						return std::get<1>(test) == value; \
					}); \
				if (iter == std::end(s_strings)) return result::failure; \
				out = std::get<0>(*iter); \
				return result::success; \
			} \
			auto get_from_string(const std::string &value) -> FullyQualifiedTypeName { \
				FullyQualifiedTypeName out; \
				if (try_get_from_string(value, out) != result::success) { \
					throw invalid_enum_exception{#FullyQualifiedTypeName}; \
				} \
				return out; \
			} \
		}; \
	} \
	;

#define ENUM_CLASS_CAST_IMPL_2(FullyQualifiedTypeName, List) \
	ENUM_CLASS_CAST_IMPL_3(FullyQualifiedTypeName, int, List)

#define ENUM_CLASS_CAST(...) \
	DISPATCH(ENUM_CLASS_CAST_IMPL_, __VA_ARGS__)
