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

#include "common/lua/config_file.hpp"
#include "common/i_packet.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include <vector>

namespace vana {
	namespace config {
		struct major_boss {
			int16_t attempts = 0;
			vector<game_channel_id> channels;
		};
	}

	template <>
	struct lua::lua_variant_into<config::major_boss> {
		auto transform(lua_environment &config, const lua_variant &obj, const string &prefix) -> config::major_boss {
			config.validate_object(lua_type::table, obj, prefix);
		
			config::major_boss ret;

			auto &values = obj.as<hash_map<lua_variant, lua_variant>>();
			for (const auto &value : values) {
				config.validate_key(lua_type::string, value.first, prefix);

				string key = value.first.as<string>();
				if (key == "attempts") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.attempts = value.second.as<int16_t>();
				}
				else if (key == "channels") {
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					auto channels = value.second.as<vector<lua_variant>>();
					for (const auto &channel : channels) {
						config.validate_value(lua_type::number, channel, "channels", prefix);
					}

					ret.channels = value.second.as<vector<game_channel_id>>();
				}
			}

			return ret;
		}
	};

	template <>
	struct packet_serialize<config::major_boss> {
		auto read(packet_reader &reader) -> config::major_boss {
			config::major_boss ret;
			ret.attempts = reader.get<int16_t>();
			ret.channels = reader.get<vector<game_channel_id>>();
			return ret;
		}
		auto write(packet_builder &builder, const config::major_boss &obj) -> void {
			builder.add<int16_t>(obj.attempts);
			builder.add<vector<game_channel_id>>(obj.channels);
		}
	};
}