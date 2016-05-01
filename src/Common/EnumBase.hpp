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

#include "Common/Preprocessor.hpp"
#include "Common/Types.hpp"
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace Vana {
	struct InvalidEnumException : public std::runtime_error {
		explicit InvalidEnumException(const std::string &message) :
			std::runtime_error{message.c_str()}
		{
		}

		explicit InvalidEnumException(const char *message = nullptr) :
			std::runtime_error{message}
		{
		}
	};

	template <typename TEnum, typename TUnderlying>
	struct EnumCaster {
		static auto tryCastFromUnderlying(TUnderlying value, TEnum &out) -> Result {
			static_assert(false, "tryCastFromUnderlying is not appropriately specialized for those types");
			throw std::logic_error{"tryCastFromUnderlying is not appropriately specialized for those types"};
		}
		static auto castFromUnderlying(TUnderlying value) -> TEnum {
			static_assert(false, "castFromUnderlying is not appropriately specialized for those types");
			throw std::logic_error{"castFromUnderlying is not appropriately specialized for those types"};
		}
		static auto tryCastToUnderlying(TEnum value, TUnderlying &out) -> Result {
			static_assert(false, "tryCastToUnderlying is not appropriately specialized for those types");
			throw std::logic_error{"tryCastToUnderlying is not appropriately specialized for those types"};
		}
		static auto castToUnderlying(TEnum value) -> TUnderlying {
			static_assert(false, "castToUnderlying is not appropriately specialized for those types");
			throw std::logic_error{"castToUnderlying is not appropriately specialized for those types"};
		}
	};

	template <typename TEnum>
	struct EnumStringifier {
		auto toString(TEnum value) -> std::string {
			static_assert(false, "toString is not appropriately specialized for that type");
			throw std::logic_error{"toString is not appropriately specialized for that type"};
		}
		auto tryGetFromString(const std::string &value, TEnum &out) -> Result {
			static_assert(false, "tryCastFromString is not appropriately specialized for that type");
			throw std::logic_error{"tryCastFromString is not appropriately specialized for that type"};
		}
		auto getFromString(const std::string &value) -> TEnum {
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
	namespace Vana { \
		template <> \
		struct EnumCaster<FullyQualifiedTypeName, UnderlyingType> { \
			static auto tryCastFromUnderlying(UnderlyingType value, FullyQualifiedTypeName &out) -> Result { \
				out = static_cast<FullyQualifiedTypeName>(value); \
				switch (out) { \
					DEFER(List)(FullyQualifiedTypeName, CASE_VALUE_IMPL) \
						return Result::Successful; \
					default: \
						return Result::Failure; \
				} \
			} \
			static auto castFromUnderlying(UnderlyingType value) -> FullyQualifiedTypeName { \
				FullyQualifiedTypeName ret; \
				if (tryCastFromUnderlying(value, ret) != Result::Successful) { \
					throw InvalidEnumException{#FullyQualifiedTypeName}; \
				} \
				return ret; \
			} \
			static auto tryCastToUnderlying(FullyQualifiedTypeName value, UnderlyingType &out) -> Result { \
				out = static_cast<UnderlyingType>(value); \
				return Result::Successful; \
			} \
			static auto castToUnderlying(FullyQualifiedTypeName value) -> UnderlyingType { \
				UnderlyingType ret; \
				if (tryCastToUnderlying(value, ret) != Result::Successful) { \
					throw InvalidEnumException{#FullyQualifiedTypeName}; \
				} \
				return ret; \
			} \
		}; \
		template <> \
		struct EnumStringifier<FullyQualifiedTypeName> { \
		private: \
			std::vector<std::tuple<FullyQualifiedTypeName, std::string>> s_strings; \
		public: \
			EnumStringifier() { \
				s_strings = { \
					DEFER(List)(FullyQualifiedTypeName, CAST_STRING_IMPL) \
				}; \
			} \
			auto toString(FullyQualifiedTypeName value) -> std::string { \
				auto iter = std::find_if( \
					std::begin(s_strings), \
					std::end(s_strings), \
					[&value](const std::tuple<FullyQualifiedTypeName, std::string> &test) -> bool { \
						return std::get<0>(test) == value; \
					}); \
				if (iter == std::end(s_strings)) return ""; \
				return std::get<1>(*iter); \
			} \
			auto tryGetFromString(const std::string &value, FullyQualifiedTypeName &out) -> Result { \
				auto iter = std::find_if( \
					std::begin(s_strings), \
					std::end(s_strings), \
					[&value](const std::tuple<FullyQualifiedTypeName, std::string> &test) -> bool { \
						return std::get<1>(test) == value; \
					}); \
				if (iter == std::end(s_strings)) return Result::Failure; \
				out = std::get<0>(*iter); \
				return Result::Successful; \
			} \
			auto getFromString(const std::string &value) -> FullyQualifiedTypeName { \
				FullyQualifiedTypeName out; \
				if (tryGetFromString(value, out) != Result::Successful) { \
					throw InvalidEnumException{#FullyQualifiedTypeName}; \
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
