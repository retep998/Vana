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

#include "common/lua/lua_type.hpp"
#include "common/types.hpp"
#include "common/util/hash_combine.hpp"
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace vana {
	namespace lua {
		class lua_environment;
		class lua_variant;

		template <typename T>
		struct lua_variant_into {
			auto transform(lua_environment &config, const lua_variant &val, const string &prefix) -> T {
				static_assert(false, "transform of T is not appropriately specialized for that type");
				throw std::logic_error{"transform of T is not appropriately specialized for that type"};
			}
		};

		// Designed to represent any structure in Lua
		class lua_variant {
		public:
			lua_variant();
			lua_variant(const lua_variant &value);
			explicit lua_variant(bool value);
			explicit lua_variant(int32_t value);
			explicit lua_variant(double value);
			explicit lua_variant(string value);
			template <typename TElement>
			explicit lua_variant(vector<TElement> value);
			template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
			explicit lua_variant(hash_map<TKey, TElement, THash, TOperation> value);
			template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
			explicit lua_variant(ord_map<TKey, TElement, TOperation> value);
			~lua_variant();

			template <typename TResult>
			auto as() const -> TResult;
			template <typename TResult>
			auto into(lua_environment &env, const string &prefix) const -> TResult;
			auto get_type() const -> lua_type;
			auto is(lua_type type) const -> bool;
			auto is_any_of(init_list<lua_type> types) const -> bool;
			auto operator =(const lua_variant &value) -> lua_variant &;
		private:
			template <typename TResult>
			auto as_impl(TResult *) const -> TResult;
			template <>
			auto as_impl<lua_variant>(lua_variant *) const -> lua_variant;
			template <>
			auto as_impl<bool>(bool *) const -> bool;
			template <>
			auto as_impl<int8_t>(int8_t *) const -> int8_t;
			template <>
			auto as_impl<uint8_t>(uint8_t *) const -> uint8_t;
			template <>
			auto as_impl<int16_t>(int16_t *) const -> int16_t;
			template <>
			auto as_impl<uint16_t>(uint16_t *) const -> uint16_t;
			template <>
			auto as_impl<int32_t>(int32_t *) const -> int32_t;
			template <>
			auto as_impl<uint32_t>(uint32_t *) const -> uint32_t;
			template <>
			auto as_impl<double>(double *) const -> double;
			template <>
			auto as_impl<string>(string *) const -> string;
			template <>
			auto as_impl<milliseconds>(milliseconds *) const -> milliseconds;
			template <>
			auto as_impl<seconds>(seconds *) const -> seconds;
			template <>
			auto as_impl<minutes>(minutes *) const -> minutes;
			template <>
			auto as_impl<hours>(hours *) const -> hours;
			template <typename TElement>
			auto as_impl(vector<TElement> *) const -> vector<TElement>;
			template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
			auto as_impl(hash_map<TKey, TElement, THash, TOperation> *) const -> hash_map<TKey, TElement, THash, TOperation>;
			template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
			auto as_impl(ord_map<TKey, TElement, TOperation> *) const -> ord_map<TKey, TElement, TOperation>;

			// The type isn't complete yet and I can't specialize a non-complete type
			// However, the type uses itself in a map in the union to complete the type
			// Therefore, just use these directly and allow std:: to use them
			friend struct ::std::hash<lua_variant>;
			struct hash {
				auto operator()(const lua_variant &v) const -> size_t {
					switch (v.get_type()) {
						case lua_type::nil: return vana::util::hash_combinator(0);
						case lua_type::boolean: return vana::util::hash_combinator(v.as<bool>() ? 0x55555555 : 1);
						case lua_type::number: return vana::util::hash_combinator(v.as<double>());
						case lua_type::string: return vana::util::hash_combinator(v.as<string>());
						default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
					}
				}
			};

			friend struct ::std::equal_to<lua_variant>;
			struct equal_to {
				auto operator()(const lua_variant &l, const lua_variant &r) const -> bool {
					if (l.get_type() != r.get_type()) return false;

					switch (l.get_type()) {
						case lua_type::nil: return true;
						case lua_type::boolean: return l.as<bool>() == r.as<bool>();
						case lua_type::number: return l.as<double>() == r.as<double>();
						case lua_type::string: return l.as<string>() == r.as<string>();
						default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
					}
				}
			};

			friend auto operator ==(const lua_variant &a, const lua_variant &b) -> bool;
			friend auto operator !=(const lua_variant &a, const lua_variant &b) -> bool;
			friend auto operator <(const lua_variant &a, const lua_variant &b) -> bool;
			friend auto operator <=(const lua_variant &a, const lua_variant &b) -> bool;
			friend auto operator >(const lua_variant &a, const lua_variant &b) -> bool;
			friend auto operator >=(const lua_variant &a, const lua_variant &b) -> bool;

			using table_map = hash_map<lua_variant, lua_variant, lua_variant::hash, lua_variant::equal_to>;

			lua_type m_type;
		#ifdef _MSC_VER
			// TODO FIXME MSVC
			// When Connect bug 1348810 is fixed, remove this stuff
			// Details: https://connect.microsoft.com/VisualStudio/feedback/details/1348810/msvc-2015-issues-spurious-diagnostic-for-unrestricted-unions
		#pragma warning(disable: 4624)
		#endif
			union {
				table_map m_table;
				string m_string;
				double m_number;
				bool m_boolean;
			};
		#ifdef _MSC_VER
		#pragma warning(default: 4624)
		#endif
		};
	}
}

namespace std {
	template <>
	struct hash<vana::lua::lua_variant> {
		auto operator()(const vana::lua::lua_variant &v) const -> size_t {
			return vana::lua::lua_variant::hash{}(v);
		}
	};

	template <>
	struct equal_to<vana::lua::lua_variant> {
		auto operator()(const vana::lua::lua_variant &l, const vana::lua::lua_variant &r) const -> bool {
			return vana::lua::lua_variant::equal_to{}(l, r);
		}
	};

	template <>
	struct less<vana::lua::lua_variant> {
		auto operator()(const vana::lua::lua_variant &l, const vana::lua::lua_variant &r) const -> bool {
			return l < r;
		}
	};

	template <>
	struct greater<vana::lua::lua_variant> {
		auto operator()(const vana::lua::lua_variant &l, const vana::lua::lua_variant &r) const -> bool {
			return l > r;
		}
	};
}

namespace vana {
	namespace lua {
		template <typename TElement>
		lua_variant::lua_variant(vector<TElement> value) :
			m_type{lua_type::table}
		{
			size_t max = value.size();
			for (size_t index = 0; index < max; index++) {
				m_table[lua_variant{static_cast<int32_t>(index)}] = value[index];
			}
		}

		template <typename TKey, typename TElement, typename THash, typename TOperation>
		lua_variant::lua_variant(hash_map<TKey, TElement, THash, TOperation> value) :
			m_type{lua_type::table}
		{
			new (&m_table) table_map{};

			if (std::is_same<TKey, lua_variant>::value && std::is_same<TElement, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[kvp.first] = kvp.second;
				}
			}
			else if (std::is_same<TKey, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[kvp.first] = lua_variant{kvp.second};
				}
			}
			else if (std::is_same<TElement, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[lua_variant{kvp.first}] = kvp.second;
				}
			}
			else {
				for (const auto &kvp : value) {
					m_table[lua_variant{kvp.first}] = lua_variant{kvp.second};
				}
			}
		}

		template <typename TKey, typename TElement, typename TOperation>
		lua_variant::lua_variant(ord_map<TKey, TElement, TOperation> value) :
			m_type{lua_type::table}
		{
			new (&m_table) table_map{};

			if (std::is_same<TKey, lua_variant>::value && std::is_same<TElement, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[kvp.first] = kvp.second;
				}
			}
			else if (std::is_same<TKey, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[kvp.first] = lua_variant{kvp.second};
				}
			}
			else if (std::is_same<TElement, lua_variant>::value) {
				for (const auto &kvp : value) {
					m_table[lua_variant{kvp.first}] = kvp.second;
				}
			}
			else {
				for (const auto &kvp : value) {
					m_table[lua_variant{kvp.first}] = lua_variant{kvp.second};
				}
			}
		}

		inline
		auto operator ==(const lua_variant &a, const lua_variant &b) -> bool {
			if (a.m_type != b.m_type) return false;
			switch (a.m_type) {
				case lua_type::nil: return true;
				case lua_type::boolean: return a.as<bool>() == b.as<bool>();
				case lua_type::number: return a.as<double>() == b.as<double>();
				case lua_type::string: return a.as<string>() == b.as<string>();
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
			}
		}

		inline
		auto operator !=(const lua_variant &a, const lua_variant &b) -> bool {
			return !(a == b);
		}

		inline
		auto operator >(const lua_variant &a, const lua_variant &b) -> bool {
			if (a.m_type != b.m_type) return false;
			switch (a.m_type) {
				case lua_type::nil: return false;
				case lua_type::boolean: return false;
				case lua_type::number: return a.as<double>() > b.as<double>();
				case lua_type::string: return a.as<string>() > b.as<string>();
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
			}
		}

		inline
		auto operator >=(const lua_variant &a, const lua_variant &b) -> bool {
			return a > b || a == b;
		}

		inline
		auto operator <(const lua_variant &a, const lua_variant &b) -> bool {
			if (a.m_type != b.m_type) return false;
			switch (a.m_type) {
				case lua_type::nil: return false;
				case lua_type::boolean: return false;
				case lua_type::number: return a.as<double>() < b.as<double>();
				case lua_type::string: return a.as<string>() < b.as<string>();
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
			}
		}

		inline
		auto operator <=(const lua_variant &a, const lua_variant &b) -> bool {
			return a < b || a == b;
		}

		template <typename TResult>
		auto lua_variant::as() const -> TResult {
			return as_impl(static_cast<TResult *>(nullptr));
		}

		template <typename TResult>
		auto lua_variant::into(lua_environment &env, const string &prefix) const -> TResult {
			lua_variant_into<TResult> x;
			return x.transform(env, *this, prefix);
		}

		template <typename TResult>
		auto lua_variant::as_impl(TResult *) const -> TResult {
			static_assert(false, "as of TResult is not appropriately specialized for that type");
			throw std::logic_error{"as of TResult is not appropriately specialized for that type"};
		}

		template <>
		auto lua_variant::as_impl<lua_variant>(lua_variant *) const -> lua_variant {
			return *this;
		}

		template <>
		auto lua_variant::as_impl<bool>(bool *) const -> bool {
			if (m_type != lua_type::boolean) throw std::invalid_argument{"Value is not a boolean"};
			return m_boolean;
		}

		template <>
		auto lua_variant::as_impl<int8_t>(int8_t *) const -> int8_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<int8_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<uint8_t>(uint8_t *) const -> uint8_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<uint8_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<int16_t>(int16_t *) const -> int16_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<int16_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<uint16_t>(uint16_t *) const -> uint16_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<uint16_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<int32_t>(int32_t *) const -> int32_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<int32_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<uint32_t>(uint32_t *) const -> uint32_t {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not an integer"};
			return static_cast<uint32_t>(m_number);
		}

		template <>
		auto lua_variant::as_impl<double>(double *) const -> double {
			if (m_type != lua_type::number) throw std::invalid_argument{"Value is not a double"};
			return m_number;
		}

		template <>
		auto lua_variant::as_impl<string>(string *) const -> string {
			if (m_type != lua_type::string) throw std::invalid_argument{"Value is not a string"};
			return m_string;
		}

		template <>
		auto lua_variant::as_impl<milliseconds>(milliseconds *) const -> milliseconds {
			return milliseconds{as<int32_t>()};
		}

		template <>
		auto lua_variant::as_impl<seconds>(seconds *) const -> seconds {
			return seconds{as<int32_t>()};
		}

		template <>
		auto lua_variant::as_impl<minutes>(minutes *) const -> minutes {
			return minutes{as<int32_t>()};
		}

		template <>
		auto lua_variant::as_impl<hours>(hours *) const -> hours {
			return hours{as<int32_t>()};
		}

		template <typename TElement>
		auto lua_variant::as_impl(vector<TElement> *) const -> vector<TElement> {
			if (m_type != lua_type::table) throw std::invalid_argument{"Value is not an array"};

			vector<TElement> values;
			auto tbl = as<ord_map<lua_variant, lua_variant>>();
			for (const auto &kvp : tbl) {
				values.push_back(kvp.second.as<TElement>());
			}
			return values;
		}

		template <typename TKey, typename TElement, typename THash, typename TOperation>
		auto lua_variant::as_impl(hash_map<TKey, TElement, THash, TOperation> *) const -> hash_map<TKey, TElement, THash, TOperation> {
			if (m_type != lua_type::table) throw std::invalid_argument{"Value is not a key-value pair store"};

			hash_map<TKey, TElement, THash, TOperation> values;
			for (const auto &kvp : m_table) {
				values[kvp.first.as<TKey>()] = kvp.second.as<TElement>();
			}
			return values;
		}

		template <typename TKey, typename TElement, typename TOperation>
		auto lua_variant::as_impl(ord_map<TKey, TElement, TOperation> *) const -> ord_map<TKey, TElement, TOperation> {
			if (m_type != lua_type::table) throw std::invalid_argument{"Value is not a key-value pair store"};

			ord_map<TKey, TElement, TOperation> values;
			for (const auto &kvp : m_table) {
				values[kvp.first.as<TKey>()] = kvp.second.as<TElement>();
			}
			return values;
		}
	}
}