/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Types.hpp"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace StringUtilities {
	auto runFlags(const opt_string_t &flags, function_t<void (const string_t &)> func) -> void;
	auto runFlags(const string_t &flags, function_t<void (const string_t &)> func) -> void;
	auto runEnum(const opt_string_t &enumText, function_t<void (const string_t &)> func) -> void;
	auto runEnum(const string_t &enumText, function_t<void (const string_t &)> func) -> void;
	auto noCaseCompare(const string_t &s1, const string_t &s2) -> int32_t;
	auto atoli(const char *str) -> int64_t;
	auto replace(const string_t &input, const string_t &what, const string_t &replacement) -> string_t;
	auto bytesToHex(const unsigned char *input, size_t inputSize, bool uppercase = true) -> string_t;
	auto split(string_t input, const string_t &delimiter) -> vector_t<string_t>;
	template <typename TElement>
	auto delimit(const string_t &delimiter, const init_list_t<TElement> &elements) -> string_t;
	template <typename TElement>
	auto delimit(const string_t &delimiter, const vector_t<TElement> &elements) -> string_t;
	template <typename Dst, typename Src>
	inline
	auto lexical_cast(const Src &arg) -> Dst;

	inline
	auto trim(string_t &s) -> string_t;
	inline
	auto leftTrim(string_t &s) -> string_t;
	inline
	auto rightTrim(string_t &s) -> string_t;
	inline
	auto toUpper(const string_t &s) -> string_t;
	inline
	auto toLower(const string_t &s) -> string_t;
	inline
	auto removeSpaces(const string_t &s) -> string_t;
}

namespace _impl {
	template <typename Dst, typename Src>
	struct lexical_cast {
		using input_type = Src;
		using output_type = Dst;
		static auto cast(const input_type &input) -> output_type {
			// Merely exists to dispatch a common interface to template specializations
			// This hopefully errors in the event that it actually gets selected and compiles
			return nullptr;
		}
	};

	template <typename Dst>
	struct lexical_cast<Dst, string_t> {
		using input_type = string_t;
		using output_type = Dst;
		static auto cast(const input_type &input) -> output_type {
			std::istringstream strm{input};
			output_type out;
			strm >> out;
			return out;
		}
	};

	template <typename Src>
	struct lexical_cast<string_t, Src> {
		using input_type = Src;
		using output_type = string_t;
		static auto cast(const input_type &input) -> output_type {
			out_stream_t strm;
			strm << input;
			return strm.str();
		}
	};

	template <>
	struct lexical_cast<uint8_t, string_t> {
		using input_type = string_t;
		using output_type = uint8_t;
		static auto cast(const input_type &input) -> output_type {
			return static_cast<output_type>(lexical_cast<uint16_t, input_type>::cast(input));
		}
	};

	template <>
	struct lexical_cast<int8_t, string_t> {
		using input_type = string_t;
		using output_type = int8_t;
		static auto cast(const input_type &input) -> output_type {
			return static_cast<output_type>(lexical_cast<int16_t, input_type>::cast(input));
		}
	};

	template <>
	struct lexical_cast<string_t, uint8_t> {
		using input_type = uint8_t;
		using output_type = string_t;
		static auto cast(const input_type &input) -> output_type {
			return lexical_cast<output_type, uint16_t>::cast(static_cast<uint16_t>(input));
		}
	};

	template <>
	struct lexical_cast<string_t, int8_t> {
		using input_type = int8_t;
		using output_type = string_t;
		static auto cast(const input_type &input) -> output_type {
			return lexical_cast<output_type, int16_t>::cast(static_cast<int16_t>(input));
		}
	};
}

template <typename TElement>
auto StringUtilities::delimit(const string_t &delimiter, const init_list_t<TElement> &elements) -> string_t {
	out_stream_t q{""};
	bool addDelimiter = false;
	for (const auto &element : elements) {
		if (addDelimiter) {
			q << delimiter;
		}
		q << element;
		addDelimiter = true;
	}
	return q.str();
}

template <typename TElement>
auto StringUtilities::delimit(const string_t &delimiter, const vector_t<TElement> &elements) -> string_t {
	out_stream_t q{""};
	bool addDelimiter = false;
	for (const auto &element : elements) {
		if (addDelimiter) {
			q << delimiter;
		}
		q << element;
		addDelimiter = true;
	}
	return q.str();
}

template <typename Dst, typename Src>
inline
auto StringUtilities::lexical_cast(const Src &arg) -> Dst {
	return _impl::lexical_cast<Dst, Src>::cast(arg);
}

inline
auto StringUtilities::trim(string_t &s) -> string_t {
	string_t r = rightTrim(s);
	return leftTrim(r);
}

inline
auto StringUtilities::leftTrim(string_t &s) -> string_t {
	s.erase(std::begin(s), std::find_if(std::begin(s), std::end(s), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

inline
auto StringUtilities::rightTrim(string_t &s) -> string_t {
	s.erase(std::find_if(std::rbegin(s), std::rend(s), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), std::end(s));
	return s;
}

inline
auto StringUtilities::toUpper(const string_t &s) -> string_t {
	string_t ret = s;
	std::transform(std::begin(ret), std::end(ret), std::begin(ret), [](char x) -> char { return static_cast<char>(std::toupper(x)); });
	return ret;
}

inline
auto StringUtilities::toLower(const string_t &s) -> string_t {
	string_t ret = s;
	std::transform(std::begin(ret), std::end(ret), std::begin(ret), [](char x) -> char { return static_cast<char>(std::tolower(x)); });
	return ret;
}

inline
auto StringUtilities::removeSpaces(const string_t &s) -> string_t {
	string_t ret = s;
	ret.erase(std::remove_if(std::begin(ret), std::end(ret), ::isspace), std::end(ret));
	return ret;
}