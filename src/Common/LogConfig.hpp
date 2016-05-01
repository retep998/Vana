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
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	struct LogConfig {
		bool log = false;
		int32_t destination = 0;
		uint32_t bufferSize = 20;
		string_t format;
		string_t timeFormat;
		string_t file;
	};

	template <>
	struct LuaSerialize<LogConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> LogConfig {
			LogConfig ret;

			LuaVariant obj = config.get<LuaVariant>(prefix);
			config.validateObject(LuaType::Table, obj, prefix);

			auto map = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasLog = false;
			bool hasDestination = false;
			bool hasFormat = false;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "log") {
					hasLog = true;
					config.validateValue(LuaType::Bool, kvp.second, key, prefix);
					ret.log = kvp.second.as<bool>();
					if (!ret.log) break;
				}
				else if (key == "destination") {
					hasDestination = true;
					config.validateValue(LuaType::Number, kvp.second, key, prefix);
					ret.destination = kvp.second.as<int32_t>();
				}
				else if (key == "format") {
					hasFormat = true;
					config.validateValue(LuaType::String, kvp.second, key, prefix);
					ret.format = kvp.second.as<string_t>();
				}
				else if (key == "buffer_size") {
					if (config.validateValue(LuaType::Number, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.bufferSize = kvp.second.as<uint32_t>();
				}
				else if (key == "file") {
					if (config.validateValue(LuaType::String, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.file = kvp.second.as<string_t>();
				}
				else if (key == "time_format") {
					if (config.validateValue(LuaType::String, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.timeFormat = kvp.second.as<string_t>();
				}
			}

			config.required(hasLog, "log", prefix);
			if (ret.log) {
				config.required(hasDestination, "destination", prefix);
				config.required(hasFormat, "format", prefix);
			}

			if (ret.timeFormat.empty()) {
				ret.timeFormat = config.get<string_t>("log_time_format");
			}

			return ret;
		}
	};
}