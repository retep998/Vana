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
#include "common/lua_environment.hpp"
#include "common/lua_variant.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	struct rates_config {
		struct types {
			static const int32_t mob_exp_rate = 0x01;
			static const int32_t quest_exp_rate = 0x02;
			static const int32_t drop_meso = 0x04;
			static const int32_t drop_rate = 0x08;
			static const int32_t global_drop_rate = 0x10;
			static const int32_t global_drop_meso = 0x20;

			static const int32_t global =
				global_drop_rate |
				global_drop_meso;

			static const int32_t all =
				mob_exp_rate |
				quest_exp_rate |
				drop_meso |
				drop_rate |
				global_drop_rate |
				global_drop_meso;
		};

		static const int32_t consistent_rate_between_global_and_regular = -1;

		int32_t mob_exp_rate = 1;
		int32_t quest_exp_rate = 1;
		int32_t drop_meso = 1;
		int32_t drop_rate = 1;
		int32_t global_drop_rate = consistent_rate_between_global_and_regular;
		int32_t global_drop_meso = consistent_rate_between_global_and_regular;

		auto is_global_drop_consistent_with_regular_drop_rate() const -> bool {
			return global_drop_rate == consistent_rate_between_global_and_regular;
		}

		auto is_global_drop_meso_consistent_with_regular_drop_meso_rate() const -> bool {
			return global_drop_meso == consistent_rate_between_global_and_regular;
		}
	};

	template <>
	struct lua_variant_into<rates_config> {
		auto transform(lua_environment &config, const lua_variant &obj, const string &prefix) -> rates_config {
			config.validate_object(lua::lua_type::table, obj, prefix);
		
			rates_config ret;

			auto &values = obj.as<hash_map<lua_variant, lua_variant>>();
			for (const auto &value : values) {
				config.validate_key(lua::lua_type::string, value.first, prefix);

				string key = value.first.as<string>();
				if (key == "mob_exp") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.mob_exp_rate = value.second.as<int32_t>();
				}
				else if (key == "quest_exp") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.quest_exp_rate = value.second.as<int32_t>();
				}
				else if (key == "drop_meso") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.drop_meso = value.second.as<int32_t>();
				}
				else if (key == "drop_rate") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.drop_rate = value.second.as<int32_t>();
				}
				else if (key == "global_drop_rate") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.global_drop_rate = value.second.as<int32_t>();
				}
				else if (key == "global_drop_meso") {
					if (config.validate_value(lua::lua_type::number, value.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.global_drop_meso = value.second.as<int32_t>();
				}
			}

			return ret;
		}
	};

	template <>
	struct packet_serialize<rates_config> {
		auto read(packet_reader &reader) -> rates_config {
			rates_config ret;
			ret.mob_exp_rate = reader.get<int32_t>();
			ret.quest_exp_rate = reader.get<int32_t>();
			ret.drop_rate = reader.get<int32_t>();
			ret.drop_meso = reader.get<int32_t>();
			ret.global_drop_rate = reader.get<int32_t>();
			ret.global_drop_meso = reader.get<int32_t>();
			return ret;
		}
		auto write(packet_builder &builder, const rates_config &obj) -> void {
			builder.add<int32_t>(obj.mob_exp_rate);
			builder.add<int32_t>(obj.quest_exp_rate);
			builder.add<int32_t>(obj.drop_rate);
			builder.add<int32_t>(obj.drop_meso);
			builder.add<int32_t>(obj.global_drop_rate);
			builder.add<int32_t>(obj.global_drop_meso);
		}
	};
}