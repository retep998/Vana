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
#include "StringUtilities.hpp"
#include "Common/Database.hpp"
#include "Common/tokenizer.hpp"
#include <iomanip>
#include <iostream>

namespace Vana {

auto StringUtilities::noCaseCompare(const string_t &s1, const string_t &s2) -> int32_t {
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

auto StringUtilities::runFlags(const opt_string_t &flags, function_t<void (const string_t &)> func) -> void {
	if (flags.is_initialized()) {
		runFlags(flags.get(), func);
	}
}

auto StringUtilities::runFlags(const string_t &flags, function_t<void (const string_t &)> func) -> void {
	if (!flags.empty()) {
		MiscUtilities::tokenizer tokens{flags, ","};
		for (const auto &token : tokens) {
			func(token);
		}
	}
}

auto StringUtilities::runEnum(const opt_string_t &enumText, function_t<void (const string_t &)> func) -> void {
	if (enumText.is_initialized()) {
		runEnum(enumText.get(), func);
	}
}

auto StringUtilities::runEnum(const string_t &enumText, function_t<void (const string_t &)> func) -> void {
	if (!enumText.empty()) {
		func(enumText);
	}
}

auto StringUtilities::atoli(const char *str) -> int64_t {
	int64_t result = 0;
	while (*str >= '0' && *str <= '9') {
		result = (result * 10) + (*str++ - '0');
	}
	return result;
}

auto StringUtilities::replace(const string_t &input, const string_t &what, const string_t &replacement) -> string_t {
	string_t ret = input;
	size_t searchLen = what.length();
	size_t foundPos = ret.find(what);
	while (foundPos != string_t::npos) {
		ret.replace(foundPos, searchLen, replacement);
		foundPos = ret.find(what); // Search the next one
	}
	return ret;
}

auto StringUtilities::bytesToHex(const unsigned char *input, size_t inputSize, bool uppercase) -> string_t {
	string_t ret;
	if (inputSize > 0) {
		out_stream_t out;
		size_t bufLen = inputSize - 1;

		for (size_t i = 0; i <= bufLen; i++) {
			out << std::hex;
			if (uppercase) {
				out << std::uppercase;
			}
			else {
				out << std::nouppercase;
			}
			out << std::setw(2) << std::setfill('0') << static_cast<int16_t>(input[i]);
			if (i < bufLen) {
				out << " ";
			}
		}

		ret = out.str();
	}
	return ret;
}

auto StringUtilities::split(string_t input, const string_t &delimiter) -> vector_t<string_t> {
	vector_t<string_t> ret;
	size_t pos = 0;
	string_t token;
	while ((pos = input.find(delimiter)) != string_t::npos) {
		token = input.substr(0, pos);
		ret.push_back(token);
		input.erase(0, pos + delimiter.length());
	}
	ret.push_back(input);
	return ret;
}

}