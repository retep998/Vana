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

#include "Common/IPacket.hpp"
#include "Common/LuaEnvironment.hpp"
#include "Common/LuaVariant.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	struct RatesConfig {
		struct Types {
			static const int32_t mobExpRate = 0x01;
			static const int32_t questExpRate = 0x02;
			static const int32_t dropMeso = 0x04;
			static const int32_t dropRate = 0x08;
			static const int32_t globalDropRate = 0x10;
			static const int32_t globalDropMeso = 0x20;

			static const int32_t global =
				globalDropRate |
				globalDropMeso;

			static const int32_t all =
				mobExpRate |
				questExpRate |
				dropMeso |
				dropRate |
				globalDropRate |
				globalDropMeso;
		};

		static const int32_t consistentRateBetweenGlobalAndRegular = -1;

		int32_t mobExpRate = 1;
		int32_t questExpRate = 1;
		int32_t dropMeso = 1;
		int32_t dropRate = 1;
		int32_t globalDropRate = consistentRateBetweenGlobalAndRegular;
		int32_t globalDropMeso = consistentRateBetweenGlobalAndRegular;

		auto isGlobalDropConsistentWithRegularDropRate() const -> bool {
			return globalDropRate == consistentRateBetweenGlobalAndRegular;
		}

		auto isGlobalDropMesoConsistentWithRegularDropMesoRate() const -> bool {
			return globalDropMeso == consistentRateBetweenGlobalAndRegular;
		}
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
				else if (key == "drop_meso") {
					if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
					ret.dropMeso = value.second.as<int32_t>();
				}
				else if (key == "drop_rate") {
					if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
					ret.dropRate = value.second.as<int32_t>();
				}
				else if (key == "global_drop_rate") {
					if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
					ret.globalDropRate = value.second.as<int32_t>();
				}
				else if (key == "global_drop_meso") {
					if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
					ret.globalDropMeso = value.second.as<int32_t>();
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
			ret.dropRate = reader.get<int32_t>();
			ret.dropMeso = reader.get<int32_t>();
			ret.globalDropRate = reader.get<int32_t>();
			ret.globalDropMeso = reader.get<int32_t>();
			return ret;
		}
		auto write(PacketBuilder &builder, const RatesConfig &obj) -> void {
			builder.add<int32_t>(obj.mobExpRate);
			builder.add<int32_t>(obj.questExpRate);
			builder.add<int32_t>(obj.dropRate);
			builder.add<int32_t>(obj.dropMeso);
			builder.add<int32_t>(obj.globalDropRate);
			builder.add<int32_t>(obj.globalDropMeso);
		}
	};
}