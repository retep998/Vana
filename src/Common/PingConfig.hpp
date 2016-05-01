/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/Types.hpp"

namespace Vana {
	struct PingConfig {
		bool enable = true;
		milliseconds_t initialDelay = milliseconds_t{60000};
		milliseconds_t interval = milliseconds_t{30000};
		int32_t timeoutPingCount = 4;
	};

	template <>
	struct LuaSerialize<PingConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> PingConfig {
			PingConfig ret;

			LuaVariant obj = config.get<LuaVariant>(prefix);
			config.validateObject(LuaType::Table, obj, prefix);

			auto map = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasEnabled = false;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "enabled") {
					hasEnabled = true;
					config.validateValue(LuaType::Bool, kvp.second, key, prefix);
					ret.enable = kvp.second.as<bool>();
				}
				else if (key == "initial_delay") {
					if (config.validateValue(LuaType::Number, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.initialDelay = milliseconds_t{kvp.second.as<int32_t>()};
				}
				else if (key == "interval") {
					if (config.validateValue(LuaType::Number, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.interval = milliseconds_t{kvp.second.as<int32_t>()};
				}
				else if (key == "timeout_ping_count") {
					if (config.validateValue(LuaType::Number, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.timeoutPingCount = kvp.second.as<int32_t>();
				}
			}

			config.required(hasEnabled, "enabled", prefix);

			return ret;
		}
	};
}