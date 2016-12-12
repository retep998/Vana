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

#include "common/config/major_boss.hpp"
#include "common/config/rates.hpp"
#include "common/i_packet.hpp"
#include "common/lua/config_file.hpp"
#include "common/lua/lua_variant.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace config {
		struct world {
			bool default_gm_chat_mode = true;
			game_world_id id = 0;
			int8_t ribbon = 0;
			connection_port base_port = 7100;
			game_player_level max_multi_level = 1;
			game_storage_slot default_storage_slots = 4;
			game_stat max_stats = 999;
			int32_t default_chars = 3;
			int32_t max_chars = 6;
			int32_t max_player_load = 1000;
			seconds fame_time = seconds{24 * 60 * 60};
			seconds fame_reset_time = seconds{24 * 60 * 60 * 30};
			seconds map_unload_time = seconds{30 * 60};
			game_channel_id max_channels = 19;
			string event_message;
			string scrolling_header;
			string name;
			rates rates;
			hash_map<game_npc_id, string> npc_forced_script;
			major_boss pianus;
			major_boss papulatus;
			major_boss zakum;
			major_boss horntail;
			major_boss pinkbean;
		};
	}

	template <>
	struct lua::lua_variant_into<config::world> {
		auto expand_major_boss(const config::world &config, config::major_boss &boss) -> void {
			if (boss.channels.size() == 1 && boss.channels[0] == -1) {
				boss.channels.clear();
				for (game_channel_id i = 1; i <= config.max_channels; i++) {
					boss.channels.push_back(static_cast<game_channel_id>(i));
				}
			}
		}

		auto transform(lua_environment &config, const lua_variant &obj, const string &prefix) -> config::world {
			config.validate_object(lua_type::table, obj, prefix);
		
			config::world ret;

			auto &values = obj.as<hash_map<lua_variant, lua_variant>>();
			bool has_name = false;
			bool has_port = false;
			bool has_id = false;
			bool has_pianus = false;
			bool has_papulatus = false;
			bool has_zakum = false;
			bool has_horntail = false;
			bool has_pinkbean = false;
			for (const auto &value : values) {
				config.validate_key(lua_type::string, value.first, prefix);

				string key = value.first.as<string>();
				if (key == "name") {
					has_name = true;
					config.validate_value(lua_type::string, value.second, key, prefix);
					ret.name = value.second.as<string>();
				}
				else if (key == "id") {
					has_id = true;
					config.validate_value(lua_type::number, value.second, key, prefix);
					ret.id = value.second.as<game_world_id>();
				}
				else if (key == "port") {
					has_port = true;
					config.validate_value(lua_type::number, value.second, key, prefix);
					ret.base_port = value.second.as<connection_port>();
				}
				else if (key == "gm_chat_by_default") {
					if (config.validate_value(lua_type::boolean, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.default_gm_chat_mode = value.second.as<bool>();
				}
				else if (key == "channels") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.max_channels = value.second.as<game_channel_id>();
				}
				else if (key == "ribbon") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.ribbon = value.second.as<int8_t>();
				}
				else if (key == "max_stats") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.max_stats = value.second.as<game_stat>();
				}
				else if (key == "max_multi_level") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.max_multi_level = value.second.as<game_player_level>();
				}
				else if (key == "event_message") {
					if (config.validate_value(lua_type::string, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.event_message = value.second.as<string>();
				}
				else if (key == "scrolling_header") {
					if (config.validate_value(lua_type::string, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.scrolling_header = value.second.as<string>();
				}
				else if (key == "max_player_load") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.max_player_load = value.second.as<int32_t>();
				}
				else if (key == "max_characters") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.max_chars = value.second.as<int32_t>();
				}
				else if (key == "default_characters") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.default_chars = value.second.as<int32_t>();
				}
				else if (key == "default_storage") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.default_storage_slots = value.second.as<game_storage_slot>();
				}
				else if (key == "fame_time") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.fame_time = value.second.as<seconds>();
				}
				else if (key == "fame_reset_time") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.fame_reset_time = value.second.as<seconds>();
				}
				else if (key == "map_unload_time") {
					if (config.validate_value(lua_type::number, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.map_unload_time = value.second.as<seconds>();
				}
				else if (key == "rates") {
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.rates = value.second.into<config::rates>(config, prefix + "." + key);
				}
				else if (key == "npc_forced_script") {
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					
					auto &values = value.second.as<hash_map<lua_variant, lua_variant>>();
					for (const auto &value : values) {
						config.validate_key(lua_type::number, value.first, prefix + "." + key);
						config.validate_key(lua_type::string, value.second, prefix + "." + key);

						ret.npc_forced_script.insert_or_assign(
							value.first.as<game_npc_id>(),
							value.second.as<string>()
						);
					}
				}
				else if (key == "pianus") {
					has_pianus = true;
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.pianus = value.second.into<config::major_boss>(config, prefix + "." + key);
				}
				else if (key == "papulatus") {
					has_papulatus = true;
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.papulatus = value.second.into<config::major_boss>(config, prefix + "." + key);
				}
				else if (key == "zakum") {
					has_zakum = true;
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.zakum = value.second.into<config::major_boss>(config, prefix + "." + key);
				}
				else if (key == "horntail") {
					has_horntail = true;
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.horntail = value.second.into<config::major_boss>(config, prefix + "." + key);
				}
				else if (key == "pinkbean") {
					has_pinkbean = true;
					if (config.validate_value(lua_type::table, value.second, key, prefix, true) == lua_type::nil) continue;
					ret.pinkbean = value.second.into<config::major_boss>(config, prefix + "." + key);
				}
			}

			config.required(has_name, "name", prefix);
			config.required(has_id, "id", prefix);
			config.required(has_port, "port", prefix);

			if (has_pianus) expand_major_boss(ret, ret.pianus);
			if (has_papulatus) expand_major_boss(ret, ret.papulatus);
			if (has_zakum) expand_major_boss(ret, ret.zakum);
			if (has_horntail) expand_major_boss(ret, ret.horntail);
			if (has_pinkbean) expand_major_boss(ret, ret.pinkbean);

			return ret;
		}
	};

	template <>
	struct packet_serialize<config::world> {
		auto read(packet_reader &reader) -> config::world {
			config::world ret;
			ret.id = reader.get<game_world_id>();
			ret.base_port = reader.get<connection_port>();
			ret.default_gm_chat_mode = reader.get<bool>();
			ret.ribbon = reader.get<int8_t>();
			ret.max_multi_level = reader.get<game_player_level>();
			ret.default_storage_slots = reader.get<game_storage_slot>();
			ret.max_stats = reader.get<game_stat>();
			ret.default_chars = reader.get<int32_t>();
			ret.max_chars = reader.get<int32_t>();
			ret.max_player_load = reader.get<int32_t>();
			ret.fame_time = reader.get<seconds>();
			ret.fame_reset_time = reader.get<seconds>();
			ret.map_unload_time = reader.get<seconds>();
			ret.max_channels = reader.get<game_channel_id>();
			ret.event_message = reader.get<string>();
			ret.scrolling_header = reader.get<string>();
			ret.name = reader.get<string>();
			ret.rates = reader.get<config::rates>();
			uint32_t npc_forced_scripts = reader.get<uint32_t>();
			for (uint32_t i = 0; i < npc_forced_scripts; i++) {
				ret.npc_forced_script.insert_or_assign(
					reader.get<game_npc_id>(),
					reader.get<string>()
				);
			}
			ret.pianus = reader.get<config::major_boss>();
			ret.papulatus = reader.get<config::major_boss>();
			ret.zakum = reader.get<config::major_boss>();
			ret.horntail = reader.get<config::major_boss>();
			ret.pinkbean = reader.get<config::major_boss>();
			return ret;
		}
		auto write(packet_builder &builder, const config::world &obj) -> void {
			builder.add<game_world_id>(obj.id);
			builder.add<connection_port>(obj.base_port);
			builder.add<bool>(obj.default_gm_chat_mode);
			builder.add<int8_t>(obj.ribbon);
			builder.add<game_player_level>(obj.max_multi_level);
			builder.add<game_storage_slot>(obj.default_storage_slots);
			builder.add<game_stat>(obj.max_stats);
			builder.add<int32_t>(obj.default_chars);
			builder.add<int32_t>(obj.max_chars);
			builder.add<int32_t>(obj.max_player_load);
			builder.add<seconds>(obj.fame_time);
			builder.add<seconds>(obj.fame_reset_time);
			builder.add<seconds>(obj.map_unload_time);
			builder.add<game_channel_id>(obj.max_channels);
			builder.add<string>(obj.event_message);
			builder.add<string>(obj.scrolling_header);
			builder.add<string>(obj.name);
			builder.add<config::rates>(obj.rates);
			builder.add<uint32_t>(obj.npc_forced_script.size());
			for (auto &kvp : obj.npc_forced_script) {
				builder.add<game_npc_id>(kvp.first);
				builder.add<string>(kvp.second);
			}
			builder.add<config::major_boss>(obj.pianus);
			builder.add<config::major_boss>(obj.papulatus);
			builder.add<config::major_boss>(obj.zakum);
			builder.add<config::major_boss>(obj.horntail);
			builder.add<config::major_boss>(obj.pinkbean);
		}
	};
}