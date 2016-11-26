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
#include "string_utilities.hpp"
#include "common/database.hpp"
#include "common/tokenizer.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace utilities {
namespace str {

auto no_case_compare(const string &s1, const string &s2) -> int32_t {
	size_t l1 = s1.size();
	size_t l2 = s2.size();
	auto iter1 = std::begin(s1);
	auto iter2 = std::begin(s2);
	auto end1 = std::end(s1);
	auto end2 = std::end(s2);

	while ((iter1 != end1) && (iter2 != end2)) {
		if (toupper(*iter1) != toupper(*iter2)) {
			return (toupper(*iter1) < toupper(*iter2)) ? -1 : 1;
		}
		++iter1;
		++iter2;
	}

	if (l1 == l2) {
		return 0;
	}

	return (l1 < l2) ? -1 : 1;
}

auto run_flags(const opt_string &flags, function<void (const string &)> func) -> void {
	if (flags.is_initialized()) {
		run_flags(flags.get(), func);
	}
}

auto run_flags(const string &flags, function<void (const string &)> func) -> void {
	if (!flags.empty()) {
		utilities::misc::tokenizer tokens{flags, ","};
		for (const auto &token : tokens) {
			func(token);
		}
	}
}

auto run_enum(const opt_string &enum_text, function<void (const string &)> func) -> void {
	if (enum_text.is_initialized()) {
		run_enum(enum_text.get(), func);
	}
}

auto run_enum(const string &enum_text, function<void (const string &)> func) -> void {
	if (!enum_text.empty()) {
		func(enum_text);
	}
}

auto atoli(const char *str) -> int64_t {
	// Discard whitespaces
	while (isspace(*str)) str++;

	bool negative = false;
	if (*str == '+') str++;
	else if (*str == '-') {
		negative = true;
		str++;
	}
	
	int64_t result = 0;
	
	while (*str >= '0' && *str <= '9') {
		result = (result * 10) + (*str++ - '0');
	}

	if (negative) result *= -1;

	return result;
}

auto replace(const string &input, const string &what, const string &replacement) -> string {
	string ret = input;
	size_t search_len = what.length();
	size_t found_pos = ret.find(what);
	while (found_pos != string::npos) {
		ret.replace(found_pos, search_len, replacement);
		found_pos = ret.find(what); // Search the next one
	}
	return ret;
}

auto bytes_to_hex(const unsigned char *input, size_t input_size, bool uppercase) -> string {
	string ret;
	if (input_size > 0) {
		out_stream out;
		size_t buffer_len = input_size - 1;

		for (size_t i = 0; i <= buffer_len; i++) {
			out << std::hex;
			if (uppercase) {
				out << std::uppercase;
			}
			else {
				out << std::nouppercase;
			}
			out << std::setw(2) << std::setfill('0') << static_cast<int16_t>(input[i]);
			if (i < buffer_len) {
				out << " ";
			}
		}

		ret = out.str();
	}
	return ret;
}

auto split(string input, const string &delimiter) -> vector<string> {
	vector<string> ret;
	size_t pos = 0;
	string token;
	while ((pos = input.find(delimiter)) != string::npos) {
		token = input.substr(0, pos);
		ret.push_back(token);
		input.erase(0, pos + delimiter.length());
	}
	ret.push_back(input);
	return ret;
}

}
}
}