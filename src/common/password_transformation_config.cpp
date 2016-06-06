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
#include "password_transformation_config.hpp"
#include "common/salt_leftover_policy.hpp"
#include "common/types.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace vana {

password_transformation_config::password_transformation_config(salt_policy policy, vector<lua::lua_variant> args) :
	m_policy{policy},
	m_args{args}
{
}

auto password_transformation_config::validate_args(salt_policy policy, const vector<lua::lua_variant> &args) -> validity_result {
	switch (policy) {
		case salt_policy::append:
		case salt_policy::prepend: {
			if (args.size() > 0) return validity_result::invalid;
			break;
		}
		case salt_policy::intersperse: {
			if (args.size() <= 1) return validity_result::invalid;
			if (args.size() >= 4) return validity_result::invalid;
			if (!args[0].is(lua::lua_type::number)) return validity_result::invalid;
			if (!args[1].is(lua::lua_type::number)) return validity_result::invalid;
			if (args.size() == 3) {
				if (!args[2].is(lua::lua_type::number)) return validity_result::invalid;
			}
			break;
		}
		case salt_policy::chop_distribute:
		case salt_policy::chop_cross_distribute: {
			if (args.size() == 0) return validity_result::invalid;
			if (args.size() >= 3) return validity_result::invalid;
			for (const auto &arg : args) {
				if (!arg.is(lua::lua_type::number)) return validity_result::invalid;
			}
			break;
		}
		default:
			throw not_implemented_exception{"salt_policy"};
	}
	return validity_result::valid;
}

auto password_transformation_config::apply(string input, string salt) const -> string {
	string ret;
	switch (m_policy) {
		case salt_policy::append:
			ret = input + salt;
			break;
		case salt_policy::prepend:
			ret = salt + input;
			break;
		case salt_policy::intersperse: {
			string intersperse = salt;
			size_t number_of_chars = m_args[0].as<uint32_t>();
			size_t offset_chars = m_args[1].as<uint32_t>();
			salt_leftover_policy policy = salt_leftover_policy::discard;
			if (m_args.size() == 3) {
				policy = static_cast<salt_leftover_policy>(m_args[2].as<int32_t>());
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
		case salt_policy::chop_distribute:
		case salt_policy::chop_cross_distribute: {
			size_t split_point_or_number_of_beginning_chars = m_args[0].as<uint32_t>();
			bool has_split_point = true;
			size_t number_of_end_chars = 0;
			if (m_args.size() == 2) {
				has_split_point = false;
				number_of_end_chars = m_args[1].as<uint32_t>();
			}

			string begin_chop = salt.substr(0, split_point_or_number_of_beginning_chars);
			string end_chop = salt.substr(
				has_split_point ?
					split_point_or_number_of_beginning_chars :
					salt.size() - number_of_end_chars,
				has_split_point ?
					string::npos :
					number_of_end_chars);

			if (m_policy == salt_policy::chop_distribute) {
				ret = begin_chop + input + end_chop;
			}
			else {
				ret = end_chop + input + begin_chop;
			}
			break;
		}
		default:
			throw not_implemented_exception{"salt_policy"};
	}
	return ret;
}

}