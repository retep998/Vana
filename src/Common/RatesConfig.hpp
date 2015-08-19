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
#include "LuaEnvironment.hpp"
#include "LuaVariant.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

struct RatesConfig {
	struct Types {
		static const int32_t MobExpRate = 0x01;
		static const int32_t QuestExpRate = 0x02;
		static const int32_t MobMesoRate = 0x04;
		static const int32_t DropRate = 0x08;
	};

	int32_t mobExpRate = 1;
	int32_t questExpRate = 1;
	int32_t mobMesoRate = 1;
	int32_t dropRate = 1;
};

template <>
struct LuaVariantInto<RatesConfig> {
	auto transform(LuaEnvironment &config, const LuaVariant &obj, const string_t &prefix) -> RatesConfig {
		config.validateObject(LuaType::Table, obj, prefix);
		
		RatesConfig ret;

		auto &values = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
		for (const auto &value : values) {
			config.validateKey(LuaType::String, value.first, prefix);

			string_t key = value.first.as<string_t>();
			if (key == "mob_exp") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.mobExpRate = value.second.as<int32_t>();
			}
			else if (key == "quest_exp") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.questExpRate = value.second.as<int32_t>();
			}
			else if (key == "mob_meso") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.mobMesoRate = value.second.as<int32_t>();
			}
			else if (key == "drop_rate") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.dropRate = value.second.as<int32_t>();
			}
		}

		return ret;
	}
};

template <>
struct PacketSerialize<RatesConfig> {
	auto read(PacketReader &reader) -> RatesConfig {
		RatesConfig ret;
		ret.mobExpRate = reader.get<int32_t>();
		ret.questExpRate = reader.get<int32_t>();
		ret.mobMesoRate = reader.get<int32_t>();
		ret.dropRate = reader.get<int32_t>();
		return ret;
	}
	auto write(PacketBuilder &builder, const RatesConfig &obj) -> void {
		builder.add<int32_t>(obj.mobExpRate);
		builder.add<int32_t>(obj.questExpRate);
		builder.add<int32_t>(obj.mobMesoRate);
		builder.add<int32_t>(obj.dropRate);
	}
};