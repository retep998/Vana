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

#include "common/config/salt.hpp"
#include "common/config/salt_size.hpp"
#include "common/config/salt_transformation.hpp"
#include "common/lua/config_file.hpp"
#include "common/salt_policy.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace config {
		struct salting {
			salt_size account_salt_size;
			salt account;
			salt interserver;	
		};
	}

	template <>
	struct lua::lua_serialize<config::salting> {
		auto read(lua_environment &config, const string &prefix) -> config::salting {
			config::salting ret;

			lua_variant account = config.get<lua_variant>("account");
			config.validate_object(lua_type::table, account, "account");

			auto map = account.as<hash_map<lua_variant, lua_variant>>();
			bool has_salt_size = false;
			for (const auto &kvp : map) {
				config.validate_key(lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "salt_size") {
					has_salt_size = true;
					ret.account_salt_size = kvp.second.into<config::salt_size>(config, prefix + "." + key);
				}
			}

			config.required(has_salt_size, "salt_size", "account");

			ret.account = config.get<config::salt>("account");
			ret.interserver = config.get<config::salt>("interserver");

			return ret;
		}
	};
}