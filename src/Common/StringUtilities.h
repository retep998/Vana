/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "Types.h"
#include <algorithm>
#include <functional>
#include <locale>
#include <sstream>
#include <string>

using std::function;
using std::string;

namespace StringUtilities {
	int32_t noCaseCompare(const string &s1, const string &s2);
	void runFlags(const opt_string &flags, function<void (const string &)> func);
	void runFlags(const string &flags, function<void (const string &)> func);
	int64_t atoli(const char *str); // ASCII to long int
	string replace(const string &input, const string &what, const string &replacement);
	template<typename T> string delimit(const string &delimiter, const T arr[], size_t arrSize);
	string bytesToHex(const unsigned char *input, size_t inputSize, bool uppercase = true);
	template<typename Dst, typename Src> inline Dst lexical_cast(const Src &arg);
	// trim from both ends
	inline string trim(string &s);
	inline string leftTrim(string &s);
	inline string rightTrim(string &s);
	inline string toUpper(const string &s);
	inline string toLower(const string &s);
}

namespace _impl {
	template<typename Dst, typename Src>
	struct lexical_cast {
		typedef Src input_type;
		typedef Dst output_type;
		static output_type cast(const input_type &input) {
			// Merely exists to dispatch a common interface to template specializations
			// This hopefully errors in the event that it actually gets selected and compiles
			return nullptr;
		}
	};

	template<typename Dst>
	struct lexical_cast<Dst, string> {
		typedef string input_type;
		typedef Dst output_type;
		static output_type cast(const input_type &input) {
			std::istringstream strm(input);
			output_type out;
			strm >> out;
			return out;
		}
	};

	template<typename Src>
	struct lexical_cast<string, Src> {
		typedef Src input_type;
		typedef string output_type;
		static output_type cast(const input_type &input) {
			std::ostringstream strm;
			strm << input;
			return strm.str();
		}
	};

	template<>
	struct lexical_cast<uint8_t, string> {
		typedef string input_type;
		typedef uint8_t output_type;
		static output_type cast(const input_type &input) {
			return static_cast<output_type>(lexical_cast<uint16_t, input_type>::cast(input));
		}
	};

	template<>
	struct lexical_cast<int8_t, string> {
		typedef string input_type;
		typedef int8_t output_type;
		static output_type cast(const input_type &input) {
			return static_cast<output_type>(lexical_cast<int16_t, input_type>::cast(input));
		}
	};

	template<>
	struct lexical_cast<string, uint8_t> {
		typedef uint8_t input_type;
		typedef string output_type;
		static output_type cast(const input_type &input) {
			return lexical_cast<output_type, uint16_t>::cast(static_cast<uint16_t>(input));
		}
	};

	template<>
	struct lexical_cast<string, int8_t> {
		typedef int8_t input_type;
		typedef string output_type;
		static output_type cast(const input_type &input) {
			return lexical_cast<output_type, int16_t>::cast(static_cast<int16_t>(input));
		}
	};
}

template<typename T>
string StringUtilities::delimit(const string &delimiter, const T arr[], size_t arrSize) {
	std::ostringstream q("");
	for (size_t i = 0; i < arrSize; ++i) {
		if (i != 0) {
			q << delimiter;
		}
		q << arr[i];
	}
	return q.str();
}

template<typename Dst, typename Src>
inline Dst StringUtilities::lexical_cast(const Src &arg) {
	return _impl::lexical_cast<Dst, Src>::cast(arg);
}

inline
string StringUtilities::trim(string &s) {
	string r = rightTrim(s);
	return leftTrim(r);
}

inline
string StringUtilities::leftTrim(string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

inline
string StringUtilities::rightTrim(string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

inline
string StringUtilities::toUpper(const string &s) {
	string ret = s;
	std::for_each(ret.begin(), ret.end(), [](char x) -> char { return std::toupper(x); });
	return ret;
}

inline
string StringUtilities::toLower(const string &s) {
	string ret = s;
	std::for_each(ret.begin(), ret.end(), [](char x) -> char { return std::tolower(x); });
	return ret;
}