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
#include "Common/PasswordTransformationConfig.hpp"
#include "Common/SaltPolicy.hpp"
#include "Common/SaltTransformationConfig.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	struct SaltConfig {
		PasswordTransformationConfig policy = {SaltPolicy::Prepend, vector_t<LuaVariant>{}};
		vector_t<SaltTransformationConfig> modifyPolicies;
	};

	template <>
	struct LuaSerialize<SaltConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> SaltConfig {
			SaltConfig ret;

			LuaVariant obj = config.get<LuaVariant>(prefix);
			config.validateObject(LuaType::Table, obj, prefix);

			auto map = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasSaltPolicy = false;
			bool hasSaltModifyPolicy = false;
			LuaVariant saltPolicy;
			LuaVariant saltModifyPolicy;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "salt") {
					hasSaltPolicy = true;
					config.validateValue(LuaType::Table, kvp.second, key, prefix);
					saltPolicy = kvp.second;
				}
				else if (key == "salt_modify") {
					hasSaltModifyPolicy = true;
					config.validateValue(LuaType::Table, kvp.second, key, prefix);
					saltModifyPolicy = kvp.second;
				}
			}

			config.required(hasSaltPolicy, "salt", prefix);
			config.required(hasSaltModifyPolicy, "salt_modify", prefix);

			ret.policy = saltPolicy.into<PasswordTransformationConfig>(config, prefix + ".salt");
		
			auto modifiers = saltModifyPolicy.as<vector_t<LuaVariant>>();
			for (const auto &modifier : modifiers) {
				ret.modifyPolicies.push_back(modifier.into<SaltTransformationConfig>(config, prefix + ".salt_modify"));
			}
			return ret;
		}
	};
}