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

#include "common/i_packet.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include <vector>

namespace vana {
	struct major_boss_config {
		int16_t attempts = 0;
		vector<game_channel_id> channels;
	};

	template <>
	struct lua_variant_into<major_boss_config> {
		auto transform(lua_environment &config, const lua_variant &obj, const string &prefix) -> major_boss_config {
			config.validate_object(lua::lua_type::table, obj, prefix);
		
			major_boss_config ret;

			auto &values = obj.as<hash_map<lua_variant, lua_variant>>();
			for (const auto &value : values) {
				config.validate_key(lua::lua_type::string, value.first, prefix);

				string key = value.first.as<string>();
				if (key == "attempts") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.attempts = value.second.as<int16_t>();
				}
				else if (key == "channels") {
					if (config.validate_value(lua::lua_type::table, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					auto channels = value.second.as<vector<lua_variant>>();
					for (const auto &channel : channels) {
						config.validate_value(lua::lua_type::number, channel, "channels", prefix);
					}

					ret.channels = value.second.as<vector<game_channel_id>>();
				}
			}

			return ret;
		}
	};

	template <>
	struct packet_serialize<major_boss_config> {
		auto read(packet_reader &reader) -> major_boss_config {
			major_boss_config ret;
			ret.attempts = reader.get<int16_t>();
			ret.channels = reader.get<vector<game_channel_id>>();
			return ret;
		}
		auto write(packet_builder &builder, const major_boss_config &obj) -> void {
			builder.add<int16_t>(obj.attempts);
			builder.add<vector<game_channel_id>>(obj.channels);
		}
	};
}