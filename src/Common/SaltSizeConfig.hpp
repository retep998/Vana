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
#include "Common/LuaVariant.hpp"
#include "Common/PasswordTransformationConfig.hpp"
#include "Common/SaltSizePolicy.hpp"
#include "Common/SaltTransformationConfig.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	struct SaltSizeConfig {
		SaltSizePolicy policy = SaltSizePolicy::Static;
		int32_t size = 10;
		int32_t min = -1;
		int32_t max = -1;
	};

	template <>
	struct LuaVariantInto<SaltSizeConfig> {
		auto transform(LuaEnvironment &config, const LuaVariant &src, const string_t &prefix) -> SaltSizeConfig {
			SaltSizeConfig ret;

			if (!src.isAny({ LuaType::Number, LuaType::Table })) {
				config.error(prefix + " must be a valid Lua number or a table");
			}

			if (src.is(LuaType::Number)) {
				int32_t size = src.as<int32_t>();
				if (size < 0) {
					config.error(prefix + " must be a non-negative number");
				}
				if (size == 0) {
					ret.policy = SaltSizePolicy::Random;
					ret.min = 10;
					ret.max = 30;
					ret.size = -1;
				}
				else {
					ret.size = size;
				}
			}
			else {
				ret.policy = SaltSizePolicy::Random;
				auto values = src.as<hash_map_t<LuaVariant, LuaVariant>>();
				bool hasMin = false;
				bool hasMax = false;
				for (const auto &kvp : values) {
					config.validateKey(LuaType::String, kvp.first, prefix);

					string_t key = kvp.first.as<string_t>();
					if (key == "min") {
						hasMin = true;
						config.validateValue(LuaType::Number, kvp.second, key, prefix);
						ret.min = kvp.second.as<int32_t>();
					}
					else if (key == "max") {
						hasMax = true;
						config.validateValue(LuaType::Number, kvp.second, key, prefix);
						ret.max = kvp.second.as<int32_t>();
					}
				}

				config.required(hasMin, "min", prefix);
				config.required(hasMax, "max", prefix);
				if (ret.min <= 0) {
					config.error(prefix + ".min must be a positive number");
				}
				if (ret.max <= 0) {
					config.error(prefix + ".max must be a positive number");
				}
				if (ret.min > ret.max) {
					config.error(prefix + ".min must be lower than " + prefix + ".max");
				}
				ret.size = -1;
			}

			return ret;
		}
	};
}