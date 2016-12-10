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
#include "salt_transformation.hpp"
#include "common/salt_leftover_policy.hpp"
#include "common/types.hpp"
#include "common/util/bit.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace vana {
namespace config {

salt_transformation::salt_transformation(salt_modify_policy policy, vector<lua::lua_variant> args) :
	m_policy{policy},
	m_args{args}
{
}

auto salt_transformation::validate_args(salt_modify_policy policy, const vector<lua::lua_variant> &args) -> validity_result {
	switch (policy) {
		case salt_modify_policy::xor_byte_cipher: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is_any_of({ lua::lua_type::number, lua::lua_type::string })) return validity_result::invalid;
			if (arg.is(lua::lua_type::string)) {
				const auto &str = arg.as<string>();
				if (str.size() != 1) return validity_result::invalid;
			}
			break;
		}
		case salt_modify_policy::xor_cipher: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is_any_of({ lua::lua_type::table, lua::lua_type::string })) return validity_result::invalid;
			if (arg.is(lua::lua_type::table)) {
				auto props = arg.as<hash_map<lua::lua_variant, lua::lua_variant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
					if (!kvp.second.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
				}
			}
			break;
		}
		case salt_modify_policy::bit_rotate_cipher:
		case salt_modify_policy::overall_bit_rotate_cipher:
		case salt_modify_policy::byte_rotate_cipher: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is(lua::lua_type::number)) return validity_result::invalid;
			break;
		}
		case salt_modify_policy::bit_reverse_cipher:
		case salt_modify_policy::reverse_cipher: {
			// Intentionally blank
			break;
		}
		case salt_modify_policy::overwrite_cipher: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is(lua::lua_type::table)) return validity_result::invalid;
			auto &map = arg.as<hash_map<lua::lua_variant, lua::lua_variant>>();
			for (const auto &kvp : map) {
				if (!kvp.first.is(lua::lua_type::number)) {
					return validity_result::invalid;
				}
				if (!kvp.second.is_any_of({ lua::lua_type::number, lua::lua_type::string })) {
					return validity_result::invalid;
				}
				if (kvp.second.is(lua::lua_type::string)) {
					if (kvp.second.as<string>().size() != 1) {
						return validity_result::invalid;
					}
				}
			}
			break;
		}
		case salt_modify_policy::add_cipher:
		case salt_modify_policy::subtract_cipher:
		case salt_modify_policy::multiply_cipher:
		case salt_modify_policy::divide_cipher: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is(lua::lua_type::number)) return validity_result::invalid;
			break;
		}
		case salt_modify_policy::append: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is_any_of({ lua::lua_type::string, lua::lua_type::table })) return validity_result::invalid;
			if (arg.is(lua::lua_type::table)) {
				auto props = arg.as<hash_map<lua::lua_variant, lua::lua_variant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
					if (!kvp.second.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
				}
			}
			break;
		}
		case salt_modify_policy::prepend: {
			if (args.size() != 1) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is_any_of({ lua::lua_type::string, lua::lua_type::table })) return validity_result::invalid;
			if (arg.is(lua::lua_type::table)) {
				auto props = arg.as<hash_map<lua::lua_variant, lua::lua_variant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
					if (!kvp.second.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
				}
			}
			break;
		}
		case salt_modify_policy::intersperse: {
			if (args.size() <= 2 || args.size() >= 5) return validity_result::invalid;
			const auto &arg = args[0];
			if (!arg.is_any_of({ lua::lua_type::string, lua::lua_type::table })) return validity_result::invalid;
			if (arg.is(lua::lua_type::table)) {
				auto props = arg.as<hash_map<lua::lua_variant, lua::lua_variant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
					if (!kvp.second.is(lua::lua_type::number)) {
						return validity_result::invalid;
					}
				}
			}

			if (!args[1].is(lua::lua_type::number)) return validity_result::invalid;
			if (!args[2].is(lua::lua_type::number)) return validity_result::invalid;
			if (args.size() == 4) {
				if (!args[3].is(lua::lua_type::number)) return validity_result::invalid;
			}
			break;
		}
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "salt_modify_policy");
	}
	return validity_result::valid;
}

auto salt_transformation::apply(string input) const -> string {
	string ret;
	switch (m_policy) {
		case salt_modify_policy::xor_cipher: {
			const auto &arg = m_args[0];
			string intersperse;
			if (arg.is(lua::lua_type::string)) {
				intersperse = m_args[0].as<string>();
			}
			else {
				const auto &bytes = arg.as<vector<uint8_t>>();
				intersperse = string{std::begin(bytes), std::end(bytes)};
			}

			size_t min_size = std::min(intersperse.size(), input.size());
			ret = input;
			for (size_t i = 0; i < min_size; i++) {
				ret[i] = ret[i] ^ intersperse[i];
			}
			break;
		}
		case salt_modify_policy::bit_rotate_cipher: {
			int32_t value = m_args[0].as<int32_t>();
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				ret[i] = vana::util::bit::rotate_right<uint8_t>(ret[i], value);
			}
			break;
		}
		case salt_modify_policy::overall_bit_rotate_cipher: {
			int32_t value = m_args[0].as<int32_t>();
			ret = input;
			if (ret.size() == 1) {
				ret[0] = vana::util::bit::rotate_right<uint8_t>(ret[0], value);
			}
			else {
				bool bit = true;
				bool temp_bit = false;
				size_t end = ret.size() - 1;
				size_t first = end;
				uint8_t bit_value = 0x01;
				uint8_t bit_match = 0x80;

				for (int32_t shift = 0; shift < value; shift++) {
					uint8_t saved = ret[first];
					for (size_t idx = end; /* Intentionally blank */; idx--) {
						temp_bit = (ret[idx] & bit_match) == bit_match;
						ret[idx] <<= 1;
						ret[idx] |= static_cast<uint8_t>(bit ? bit_value : 0);
						bit = temp_bit;
						if (idx == 0) break;
					}

					temp_bit = (saved & bit_match) == bit_match;
					saved <<= 1;
					saved |= static_cast<uint8_t>(bit ? bit_value : 0);
					ret[first] = saved;
					bit = temp_bit;
				}
			}
			break;
		}
		case salt_modify_policy::byte_rotate_cipher: {
			size_t amount = m_args[0].as<uint32_t>();
			ret = input;
			auto new_top = std::begin(ret);
			std::advance(new_top, amount);
			std::rotate(std::begin(ret), new_top, std::end(ret));
			break;
		}
		case salt_modify_policy::bit_reverse_cipher: {
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				ret[i] = vana::util::bit::reverse<uint8_t>(ret[i]);
			}
			break;
		}
		case salt_modify_policy::reverse_cipher: {
			ret = input;
			std::reverse(std::begin(ret), std::end(ret));
			break;
		}
		case salt_modify_policy::overwrite_cipher: {
			const auto &arg = m_args[0].as<hash_map<uint32_t, lua::lua_variant>>();
			ret = input;
			for (const auto &replace : arg) {
				uint8_t value;
				if (replace.second.is(lua::lua_type::number)) {
					value = replace.second.as<uint8_t>();
				}
				else {
					value = replace.second.as<string>()[0];
				}
				ret[replace.first] = value;
			}
			break;
		}
		case salt_modify_policy::xor_byte_cipher:
		case salt_modify_policy::add_cipher:
		case salt_modify_policy::subtract_cipher:
		case salt_modify_policy::multiply_cipher:
		case salt_modify_policy::divide_cipher: {
			uint8_t value = 0;
			if (m_policy == salt_modify_policy::xor_byte_cipher) {
				const auto &arg = m_args[0];
				if (arg.is(lua::lua_type::number)) {
					value = m_args[0].as<uint8_t>();
				}
				else {
					value = m_args[0].as<string>()[0];
				}
			}
			else {
				value = m_args[0].as<uint8_t>();	
			}
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				switch (m_policy) {
					case salt_modify_policy::xor_byte_cipher: ret[i] ^= value; break;
					case salt_modify_policy::add_cipher: ret[i] += value; break;
					case salt_modify_policy::subtract_cipher: ret[i] -= value; break;
					case salt_modify_policy::multiply_cipher: ret[i] *= value; break;
					case salt_modify_policy::divide_cipher: ret[i] /= value; break;
					default: THROW_CODE_EXCEPTION(not_implemented_exception, "salt_modify_policy");
				}
			}
			break;
		}
		case salt_modify_policy::append: {
			const auto &arg = m_args[0];
			if (arg.is(lua::lua_type::string)) {
				ret = input + arg.as<string>();
			}
			else {
				const auto &bytes = arg.as<vector<uint8_t>>();
				ret = input + string{std::begin(bytes), std::end(bytes)};
			}
			break;
		}
		case salt_modify_policy::prepend: {
			const auto &arg = m_args[0];
			if (arg.is(lua::lua_type::string)) {
				ret = arg.as<string>() + input;
			}
			else {
				const auto &bytes = arg.as<vector<uint8_t>>();
				ret = string{std::begin(bytes), std::end(bytes)} + input;
			}
			break;
		}
		case salt_modify_policy::intersperse: {
			const auto &arg = m_args[0];
			string intersperse;
			if (arg.is(lua::lua_type::string)) {
				intersperse = m_args[0].as<string>();
			}
			else {
				const auto &bytes = arg.as<vector<uint8_t>>();
				intersperse = string{std::begin(bytes), std::end(bytes)};
			}
			size_t number_of_chars = m_args[1].as<uint32_t>();
			size_t offset_chars = m_args[2].as<uint32_t>();
			salt_leftover_policy policy = salt_leftover_policy::discard;
			if (m_args.size() == 4) {
				policy = static_cast<salt_leftover_policy>(m_args[3].as<int32_t>());
			}

			ret = "";
			string apply = input;
			bool start_processing = false;
			size_t interspersed_pos = 0;
			do {
				size_t i = 0;
				for (/* Intentionally blank */; i < apply.size(); i++) {
					if (interspersed_pos == intersperse.size()) {
						// No more salt to apply
						break;
					}

					if (i == offset_chars) {
						start_processing = true;
					}
					else if (start_processing) {
						if (i % number_of_chars == 0) {
							ret += intersperse[interspersed_pos++];
						}
					}
					ret += apply[i];
				}

				if (policy == salt_leftover_policy::rollover) {
					if (interspersed_pos < intersperse.size()) {
						intersperse = intersperse.substr(interspersed_pos);
						apply = ret;
						ret = "";
						interspersed_pos = 0;
						continue;
					}
				}

				if (i < apply.size()) {
					ret += apply.substr(i);
				}

				break;
			}
			while (true);

			if (interspersed_pos < intersperse.size()) {
				if (policy == salt_leftover_policy::append) {
					ret = ret + intersperse.substr(interspersed_pos);
				}
				else if (policy == salt_leftover_policy::prepend) {
					ret = intersperse.substr(interspersed_pos) + ret;
				}
			}
			break;
		}
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "salt_modify_policy");
	}
	return ret;
}

}
}