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

#include "common/types.hpp"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace vana {
	namespace util {
		namespace str {
			auto run_flags(const opt_string &flags, function<void (const string &)> func) -> void;
			auto run_flags(const string &flags, function<void (const string &)> func) -> void;
			auto run_enum(const opt_string &enum_text, function<void (const string &)> func) -> void;
			auto run_enum(const string &enum_text, function<void (const string &)> func) -> void;

			auto no_case_compare(const string &s1, const string &s2) -> int32_t;
			auto atoli(const char *str) -> int64_t;
			auto replace(const string &input, const string &what, const string &replacement) -> string;
			auto bytes_to_hex(const unsigned char *input, size_t input_size, bool uppercase = true) -> string;
			auto split(string input, const string &delimiter) -> vector<string>;
			template <typename TElement>
			auto delimit(const string &delimiter, const init_list<TElement> &elements) -> string;
			template <typename TElement>
			auto delimit(const string &delimiter, const vector<TElement> &elements) -> string;
			template <typename TDst, typename TSrc>
			inline
			auto lexical_cast(const TSrc &arg) -> TDst;

			inline
			auto trim(string &s) -> string;
			inline
			auto left_trim(string &s) -> string;
			inline
			auto right_trim(string &s) -> string;
			inline
			auto to_upper(const string &s) -> string;
			inline
			auto to_lower(const string &s) -> string;
			inline
			auto remove_spaces(const string &s) -> string;
		}
	}

	namespace _impl {
		template <typename TDst, typename TSrc>
		struct lexical_cast {
			using input_type = TSrc;
			using output_type = TDst;
			static auto cast(const input_type &input) -> output_type {
				// Merely exists to dispatch a common interface to template specializations
				// This hopefully errors in the event that it actually gets selected and compiles
				return nullptr;
			}
		};

		template <typename TDst>
		struct lexical_cast<TDst, string> {
			using input_type = string;
			using output_type = TDst;
			static auto cast(const input_type &input) -> output_type {
				std::istringstream strm{input};
				output_type out;
				strm >> out;
				return out;
			}
		};

		template <typename TSrc>
		struct lexical_cast<string, TSrc> {
			using input_type = TSrc;
			using output_type = string;
			static auto cast(const input_type &input) -> output_type {
				out_stream strm;
				strm << input;
				return strm.str();
			}
		};

		template <>
		struct lexical_cast<uint8_t, string> {
			using input_type = string;
			using output_type = uint8_t;
			static auto cast(const input_type &input) -> output_type {
				return static_cast<output_type>(lexical_cast<uint16_t, input_type>::cast(input));
			}
		};

		template <>
		struct lexical_cast<int8_t, string> {
			using input_type = string;
			using output_type = int8_t;
			static auto cast(const input_type &input) -> output_type {
				return static_cast<output_type>(lexical_cast<int16_t, input_type>::cast(input));
			}
		};

		template <>
		struct lexical_cast<string, uint8_t> {
			using input_type = uint8_t;
			using output_type = string;
			static auto cast(const input_type &input) -> output_type {
				return lexical_cast<output_type, uint16_t>::cast(static_cast<uint16_t>(input));
			}
		};

		template <>
		struct lexical_cast<string, int8_t> {
			using input_type = int8_t;
			using output_type = string;
			static auto cast(const input_type &input) -> output_type {
				return lexical_cast<output_type, int16_t>::cast(static_cast<int16_t>(input));
			}
		};
	}

	template <typename TElement>
	auto util::str::delimit(const string &delimiter, const init_list<TElement> &elements) -> string {
		out_stream q{""};
		bool add_delimiter = false;
		for (const auto &element : elements) {
			if (add_delimiter) {
				q << delimiter;
			}
			q << element;
			add_delimiter = true;
		}
		return q.str();
	}

	template <typename TElement>
	auto util::str::delimit(const string &delimiter, const vector<TElement> &elements) -> string {
		out_stream q{""};
		bool add_delimiter = false;
		for (const auto &element : elements) {
			if (add_delimiter) {
				q << delimiter;
			}
			q << element;
			add_delimiter = true;
		}
		return q.str();
	}

	template <typename TDst, typename TSrc>
	inline
	auto util::str::lexical_cast(const TSrc &arg) -> TDst {
		return _impl::lexical_cast<TDst, TSrc>::cast(arg);
	}

	inline
	auto util::str::trim(string &s) -> string {
		string r = right_trim(s);
		return left_trim(r);
	}

	inline
	auto util::str::left_trim(string &s) -> string {
		s.erase(std::begin(s), std::find_if(std::begin(s), std::end(s), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	inline
	auto util::str::right_trim(string &s) -> string {
		s.erase(std::find_if(std::rbegin(s), std::rend(s), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), std::end(s));
		return s;
	}

	inline
	auto util::str::to_upper(const string &s) -> string {
		string ret = s;
		std::transform(std::begin(ret), std::end(ret), std::begin(ret), [](char x) -> char { return static_cast<char>(std::toupper(x)); });
		return ret;
	}

	inline
	auto util::str::to_lower(const string &s) -> string {
		string ret = s;
		std::transform(std::begin(ret), std::end(ret), std::begin(ret), [](char x) -> char { return static_cast<char>(std::tolower(x)); });
		return ret;
	}

	inline
	auto util::str::remove_spaces(const string &s) -> string {
		string ret = s;
		ret.erase(std::remove_if(std::begin(ret), std::end(ret), ::isspace), std::end(ret));
		return ret;
	}
}