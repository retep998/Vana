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

#include "Common/hash_combine.hpp"
#include "Common/LuaType.hpp"
#include "Common/Types.hpp"
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace Vana {
	class LuaEnvironment;
	class LuaVariant;

	template <typename T>
	struct LuaVariantInto {
		auto transform(LuaEnvironment &config, const LuaVariant &val, const string_t &prefix) -> T {
			static_assert(false, "transform of T is not appropriately specialized for that type");
			throw std::logic_error{"transform of T is not appropriately specialized for that type"};
		}
	};

	// Designed to represent any structure in Lua
	class LuaVariant {
	public:
		LuaVariant();
		LuaVariant(const LuaVariant &value);
		explicit LuaVariant(bool value);
		explicit LuaVariant(int32_t value);
		explicit LuaVariant(double value);
		explicit LuaVariant(string_t value);
		template <typename TElement>
		explicit LuaVariant(vector_t<TElement> value);
		template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
		explicit LuaVariant(hash_map_t<TKey, TElement, THash, TOperation> value);
		template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
		explicit LuaVariant(ord_map_t<TKey, TElement, TOperation> value);
		~LuaVariant();

		template <typename TResult>
		auto as() const -> TResult;
		template <typename TResult>
		auto into(LuaEnvironment &env, const string_t &prefix) const -> TResult;
		auto getType() const -> LuaType;
		auto is(LuaType type) const -> bool;
		auto isAny(init_list_t<LuaType> types) const -> bool;
		auto operator =(const LuaVariant &value) -> LuaVariant &;
	private:
		template <typename TResult>
		auto asImpl(TResult *) const -> TResult;
		template <>
		auto asImpl<LuaVariant>(LuaVariant *) const -> LuaVariant;
		template <>
		auto asImpl<bool>(bool *) const -> bool;
		template <>
		auto asImpl<int8_t>(int8_t *) const -> int8_t;
		template <>
		auto asImpl<uint8_t>(uint8_t *) const -> uint8_t;
		template <>
		auto asImpl<int16_t>(int16_t *) const -> int16_t;
		template <>
		auto asImpl<uint16_t>(uint16_t *) const -> uint16_t;
		template <>
		auto asImpl<int32_t>(int32_t *) const -> int32_t;
		template <>
		auto asImpl<uint32_t>(uint32_t *) const -> uint32_t;
		template <>
		auto asImpl<double>(double *) const -> double;
		template <>
		auto asImpl<string_t>(string_t *) const -> string_t;
		template <>
		auto asImpl<milliseconds_t>(milliseconds_t *) const -> milliseconds_t;
		template <>
		auto asImpl<seconds_t>(seconds_t *) const -> seconds_t;
		template <>
		auto asImpl<minutes_t>(minutes_t *) const -> minutes_t;
		template <>
		auto asImpl<hours_t>(hours_t *) const -> hours_t;
		template <typename TElement>
		auto asImpl(vector_t<TElement> *) const -> vector_t<TElement>;
		template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
		auto asImpl(hash_map_t<TKey, TElement, THash, TOperation> *) const -> hash_map_t<TKey, TElement, THash, TOperation>;
		template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
		auto asImpl(ord_map_t<TKey, TElement, TOperation> *) const -> ord_map_t<TKey, TElement, TOperation>;

		// The type isn't complete yet and I can't specialize a non-complete type
		// However, the type uses itself in a map in the union to complete the type
		// Therefore, just use these directly and allow std:: to use them
		friend struct ::std::hash<LuaVariant>;
		struct hash {
			auto operator()(const LuaVariant &v) const -> size_t {
				switch (v.getType()) {
					case LuaType::Nil: return MiscUtilities::hash_combinator(0);
					case LuaType::Bool: return MiscUtilities::hash_combinator(v.as<bool>() ? 0x55555555 : 1);
					case LuaType::Number: return MiscUtilities::hash_combinator(v.as<double>());
					case LuaType::String: return MiscUtilities::hash_combinator(v.as<string_t>());
					default: throw NotImplementedException{"LuaType"};
				}
			}
		};

		friend struct ::std::equal_to<LuaVariant>;
		struct equal_to {
			auto operator()(const LuaVariant &l, const LuaVariant &r) const -> bool {
				if (l.getType() != r.getType()) return false;

				switch (l.getType()) {
					case LuaType::Nil: return true;
					case LuaType::Bool: return l.as<bool>() == r.as<bool>();
					case LuaType::Number: return l.as<double>() == r.as<double>();
					case LuaType::String: return l.as<string_t>() == r.as<string_t>();
					default: throw NotImplementedException{"LuaType"};
				}
			}
		};

		friend auto operator ==(const LuaVariant &a, const LuaVariant &b) -> bool;
		friend auto operator !=(const LuaVariant &a, const LuaVariant &b) -> bool;
		friend auto operator <(const LuaVariant &a, const LuaVariant &b) -> bool;
		friend auto operator <=(const LuaVariant &a, const LuaVariant &b) -> bool;
		friend auto operator >(const LuaVariant &a, const LuaVariant &b) -> bool;
		friend auto operator >=(const LuaVariant &a, const LuaVariant &b) -> bool;

		using table_map_t = hash_map_t<LuaVariant, LuaVariant, LuaVariant::hash, LuaVariant::equal_to>;

		LuaType m_type;
	#ifdef _MSC_VER
		// TODO FIXME MSVC
		// When Connect bug 1348810 is fixed, remove this stuff
		// Details: https://connect.microsoft.com/VisualStudio/feedback/details/1348810/msvc-2015-issues-spurious-diagnostic-for-unrestricted-unions
	#pragma warning(disable: 4624)
	#endif
		union {
			table_map_t m_table;
			string_t m_string;
			double m_number;
			bool m_boolean;
		};
	#ifdef _MSC_VER
	#pragma warning(default: 4624)
	#endif
	};
}

namespace std {
	template <>
	struct hash<Vana::LuaVariant> {
		auto operator()(const Vana::LuaVariant &v) const -> size_t {
			return Vana::LuaVariant::hash{}(v);
		}
	};

	template <>
	struct equal_to<Vana::LuaVariant> {
		auto operator()(const Vana::LuaVariant &l, const Vana::LuaVariant &r) const -> bool {
			return Vana::LuaVariant::equal_to{}(l, r);
		}
	};

	template <>
	struct less<Vana::LuaVariant> {
		auto operator()(const Vana::LuaVariant &l, const Vana::LuaVariant &r) const -> bool {
			return l < r;
		}
	};

	template <>
	struct greater<Vana::LuaVariant> {
		auto operator()(const Vana::LuaVariant &l, const Vana::LuaVariant &r) const -> bool {
			return l > r;
		}
	};
}

namespace Vana {
	template <typename TElement>
	LuaVariant::LuaVariant(vector_t<TElement> value) :
		m_type{LuaType::Table}
	{
		size_t max = value.size();
		for (size_t index = 0; index < max; index++) {
			m_table[LuaVariant{static_cast<int32_t>(index)}] = value[index];
		}
	}

	template <typename TKey, typename TElement, typename THash, typename TOperation>
	LuaVariant::LuaVariant(hash_map_t<TKey, TElement, THash, TOperation> value) :
		m_type{LuaType::Table}
	{
		new (&m_table) table_map_t{};

		if (std::is_same<TKey, LuaVariant>::value && std::is_same<TElement, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[kvp.first] = kvp.second;
			}
		}
		else if (std::is_same<TKey, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[kvp.first] = LuaVariant{kvp.second};
			}
		}
		else if (std::is_same<TElement, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[LuaVariant{kvp.first}] = kvp.second;
			}
		}
		else {
			for (const auto &kvp : value) {
				m_table[LuaVariant{kvp.first}] = LuaVariant{kvp.second};
			}
		}
	}

	template <typename TKey, typename TElement, typename TOperation>
	LuaVariant::LuaVariant(ord_map_t<TKey, TElement, TOperation> value) :
		m_type{LuaType::Table}
	{
		new (&m_table) table_map_t{};

		if (std::is_same<TKey, LuaVariant>::value && std::is_same<TElement, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[kvp.first] = kvp.second;
			}
		}
		else if (std::is_same<TKey, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[kvp.first] = LuaVariant{kvp.second};
			}
		}
		else if (std::is_same<TElement, LuaVariant>::value) {
			for (const auto &kvp : value) {
				m_table[LuaVariant{kvp.first}] = kvp.second;
			}
		}
		else {
			for (const auto &kvp : value) {
				m_table[LuaVariant{kvp.first}] = LuaVariant{kvp.second};
			}
		}
	}

	inline
	auto operator ==(const LuaVariant &a, const LuaVariant &b) -> bool {
		if (a.m_type != b.m_type) return false;
		switch (a.m_type) {
			case LuaType::Nil: return true;
			case LuaType::Bool: return a.as<bool>() == b.as<bool>();
			case LuaType::Number: return a.as<double>() == b.as<double>();
			case LuaType::String: return a.as<string_t>() == b.as<string_t>();
			default: throw NotImplementedException{"LuaType"};
		}
	}

	inline
	auto operator !=(const LuaVariant &a, const LuaVariant &b) -> bool {
		return !(a == b);
	}

	inline
	auto operator >(const LuaVariant &a, const LuaVariant &b) -> bool {
		if (a.m_type != b.m_type) return false;
		switch (a.m_type) {
			case LuaType::Nil: return false;
			case LuaType::Bool: return false;
			case LuaType::Number: return a.as<double>() > b.as<double>();
			case LuaType::String: return a.as<string_t>() > b.as<string_t>();
			default: throw NotImplementedException{"LuaType"};
		}
	}

	inline
	auto operator >=(const LuaVariant &a, const LuaVariant &b) -> bool {
		return a > b || a == b;
	}

	inline
	auto operator <(const LuaVariant &a, const LuaVariant &b) -> bool {
		if (a.m_type != b.m_type) return false;
		switch (a.m_type) {
			case LuaType::Nil: return false;
			case LuaType::Bool: return false;
			case LuaType::Number: return a.as<double>() < b.as<double>();
			case LuaType::String: return a.as<string_t>() < b.as<string_t>();
			default: throw NotImplementedException{"LuaType"};
		}
	}

	inline
	auto operator <=(const LuaVariant &a, const LuaVariant &b) -> bool {
		return a < b || a == b;
	}

	template <typename TResult>
	auto LuaVariant::as() const -> TResult {
		return asImpl(static_cast<TResult *>(nullptr));
	}

	template <typename TResult>
	auto LuaVariant::into(LuaEnvironment &env, const string_t &prefix) const -> TResult {
		LuaVariantInto<TResult> x;
		return x.transform(env, *this, prefix);
	}

	template <typename TResult>
	auto LuaVariant::asImpl(TResult *) const -> TResult {
		static_assert(false, "as of TResult is not appropriately specialized for that type");
		throw std::logic_error{"as of TResult is not appropriately specialized for that type"};
	}

	template <>
	auto LuaVariant::asImpl<LuaVariant>(LuaVariant *) const -> LuaVariant {
		return *this;
	}

	template <>
	auto LuaVariant::asImpl<bool>(bool *) const -> bool {
		if (m_type != LuaType::Bool) throw std::invalid_argument{"Value is not a boolean"};
		return m_boolean;
	}

	template <>
	auto LuaVariant::asImpl<int8_t>(int8_t *) const -> int8_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<int8_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<uint8_t>(uint8_t *) const -> uint8_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<uint8_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<int16_t>(int16_t *) const -> int16_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<int16_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<uint16_t>(uint16_t *) const -> uint16_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<uint16_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<int32_t>(int32_t *) const -> int32_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<int32_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<uint32_t>(uint32_t *) const -> uint32_t {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not an integer"};
		return static_cast<uint32_t>(m_number);
	}

	template <>
	auto LuaVariant::asImpl<double>(double *) const -> double {
		if (m_type != LuaType::Number) throw std::invalid_argument{"Value is not a double"};
		return m_number;
	}

	template <>
	auto LuaVariant::asImpl<string_t>(string_t *) const -> string_t {
		if (m_type != LuaType::String) throw std::invalid_argument{"Value is not a string"};
		return m_string;
	}

	template <>
	auto LuaVariant::asImpl<milliseconds_t>(milliseconds_t *) const -> milliseconds_t {
		return milliseconds_t{as<int32_t>()};
	}

	template <>
	auto LuaVariant::asImpl<seconds_t>(seconds_t *) const -> seconds_t {
		return seconds_t{as<int32_t>()};
	}

	template <>
	auto LuaVariant::asImpl<minutes_t>(minutes_t *) const -> minutes_t {
		return minutes_t{as<int32_t>()};
	}

	template <>
	auto LuaVariant::asImpl<hours_t>(hours_t *) const -> hours_t {
		return hours_t{as<int32_t>()};
	}

	template <typename TElement>
	auto LuaVariant::asImpl(vector_t<TElement> *) const -> vector_t<TElement> {
		if (m_type != LuaType::Table) throw std::invalid_argument{"Value is not an array"};

		vector_t<TElement> values;
		auto tbl = as<ord_map_t<LuaVariant, LuaVariant>>();
		for (const auto &kvp : tbl) {
			values.push_back(kvp.second.as<TElement>());
		}
		return values;
	}

	template <typename TKey, typename TElement, typename THash, typename TOperation>
	auto LuaVariant::asImpl(hash_map_t<TKey, TElement, THash, TOperation> *) const -> hash_map_t<TKey, TElement, THash, TOperation> {
		if (m_type != LuaType::Table) throw std::invalid_argument{"Value is not a key-value pair store"};

		hash_map_t<TKey, TElement, THash, TOperation> values;
		for (const auto &kvp : m_table) {
			values[kvp.first.as<TKey>()] = kvp.second.as<TElement>();
		}
		return values;
	}

	template <typename TKey, typename TElement, typename TOperation>
	auto LuaVariant::asImpl(ord_map_t<TKey, TElement, TOperation> *) const -> ord_map_t<TKey, TElement, TOperation> {
		if (m_type != LuaType::Table) throw std::invalid_argument{"Value is not a key-value pair store"};

		ord_map_t<TKey, TElement, TOperation> values;
		for (const auto &kvp : m_table) {
			values[kvp.first.as<TKey>()] = kvp.second.as<TElement>();
		}
		return values;
	}
}