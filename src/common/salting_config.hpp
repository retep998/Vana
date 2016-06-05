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

#include "common/config_file.hpp"
#include "common/salt_config.hpp"
#include "common/salt_policy.hpp"
#include "common/salt_size_config.hpp"
#include "common/salt_transformation_config.hpp"
#include "common/types_temp.hpp"
#include <string>
#include <vector>

namespace vana {
	struct salting_config {
		salt_size_config account_salt_size;
		salt_config account;
		salt_config interserver;	
	};

	template <>
	struct lua_serialize<salting_config> {
		auto read(lua_environment &config, const string &prefix) -> salting_config {
			salting_config ret;

			lua_variant account = config.get<lua_variant>("account");
			config.validate_object(lua::lua_type::table, account, "account");

			auto map = account.as<hash_map<lua_variant, lua_variant>>();
			bool has_salt_size = false;
			for (const auto &kvp : map) {
				config.validate_key(lua::lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "salt_size") {
					has_salt_size = true;
					ret.account_salt_size = kvp.second.into<salt_size_config>(config, prefix + "." + key);
				}
			}

			config.required(has_salt_size, "salt_size", "account");

			ret.account = config.get<salt_config>("account");
			ret.interserver = config.get<salt_config>("interserver");

			return ret;
		}
	};
}