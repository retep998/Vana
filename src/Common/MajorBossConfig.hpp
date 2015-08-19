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

#include "IPacket.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"
#include <vector>

struct MajorBossConfig {
	int16_t attempts = 0;
	vector_t<channel_id_t> channels;
};

template <>
struct LuaVariantInto<MajorBossConfig> {
	auto transform(LuaEnvironment &config, const LuaVariant &obj, const string_t &prefix) -> MajorBossConfig {
		config.validateObject(LuaType::Table, obj, prefix);
		
		MajorBossConfig ret;

		auto &values = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
		for (const auto &value : values) {
			config.validateKey(LuaType::String, value.first, prefix);

			string_t key = value.first.as<string_t>();
			if (key == "attempts") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.attempts = value.second.as<int16_t>();
			}
			else if (key == "channels") {
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				auto channels = value.second.as<vector_t<LuaVariant>>();
				for (const auto &channel : channels) {
					config.validateValue(LuaType::Number, channel, "channels", prefix);
				}

				ret.channels = value.second.as<vector_t<channel_id_t>>();
			}
		}

		return ret;
	}
};

template <>
struct PacketSerialize<MajorBossConfig> {
	auto read(PacketReader &reader) -> MajorBossConfig {
		MajorBossConfig ret;
		ret.attempts = reader.get<int16_t>();
		ret.channels = reader.get<vector_t<channel_id_t>>();
		return ret;
	}
	auto write(PacketBuilder &builder, const MajorBossConfig &obj) -> void {
		builder.add<int16_t>(obj.attempts);
		builder.add<vector_t<channel_id_t>>(obj.channels);
	}
};