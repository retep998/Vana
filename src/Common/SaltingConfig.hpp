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

#include "Common/ConfigFile.hpp"
#include "Common/SaltConfig.hpp"
#include "Common/SaltPolicy.hpp"
#include "Common/SaltSizeConfig.hpp"
#include "Common/SaltTransformationConfig.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	struct SaltingConfig {
		SaltSizeConfig accountSaltSize;
		SaltConfig account;
		SaltConfig interserver;	
	};

	template <>
	struct LuaSerialize<SaltingConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> SaltingConfig {
			SaltingConfig ret;

			LuaVariant account = config.get<LuaVariant>("account");
			config.validateObject(LuaType::Table, account, "account");

			auto map = account.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasSaltSize = false;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "salt_size") {
					hasSaltSize = true;
					ret.accountSaltSize = kvp.second.into<SaltSizeConfig>(config, prefix + "." + key);
				}
			}

			config.required(hasSaltSize, "salt_size", "account");

			ret.account = config.get<SaltConfig>("account");
			ret.interserver = config.get<SaltConfig>("interserver");

			return ret;
		}
	};
}