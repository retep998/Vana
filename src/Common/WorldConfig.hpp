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

#include "ConfigFile.hpp"
#include "IPacket.hpp"
#include "LuaVariant.hpp"
#include "MajorBossConfig.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "RatesConfig.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

struct WorldConfig {
	bool defaultGmChatMode = true;
	world_id_t id = 0;
	int8_t ribbon = 0;
	port_t basePort = 7100;
	player_level_t maxMultiLevel = 1;
	storage_slot_t defaultStorageSlots = 4;
	stat_t maxStats = 999;
	int32_t defaultChars = 3;
	int32_t maxChars = 6;
	int32_t maxPlayerLoad = 100;
	seconds_t fameTime = seconds_t{24 * 60 * 60};
	seconds_t fameResetTime = seconds_t{24 * 60 * 60 * 30};
	seconds_t mapUnloadTime = seconds_t{30 * 60};
	channel_id_t maxChannels = 20;
	string_t eventMessage;
	string_t scrollingHeader;
	string_t name;
	RatesConfig rates;
	MajorBossConfig pianus;
	MajorBossConfig papulatus;
	MajorBossConfig zakum;
	MajorBossConfig horntail;
	MajorBossConfig pinkbean;
};

template <>
struct LuaVariantInto<WorldConfig> {
	auto expandMajorBoss(const WorldConfig &config, MajorBossConfig &boss) -> void {
		if (boss.channels.size() == 1 && boss.channels[0] == -1) {
			boss.channels.clear();
			for (channel_id_t i = 1; i <= config.maxChannels; i++) {
				boss.channels.push_back(static_cast<channel_id_t>(i));
			}
		}
	}

	auto transform(LuaEnvironment &config, const LuaVariant &obj, const string_t &prefix) -> WorldConfig {
		config.validateObject(LuaType::Table, obj, prefix);
		
		WorldConfig ret;

		auto &values = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
		bool hasName = false;
		bool hasPort = false;
		bool hasId = false;
		bool hasPianus = false;
		bool hasPapulatus = false;
		bool hasZakum = false;
		bool hasHorntail = false;
		bool hasPinkbean = false;
		for (const auto &value : values) {
			config.validateKey(LuaType::String, value.first, prefix);

			string_t key = value.first.as<string_t>();
			if (key == "name") {
				hasName = true;
				config.validateValue(LuaType::String, value.second, key, prefix);
				ret.name = value.second.as<string_t>();
			}
			else if (key == "id") {
				hasId = true;
				config.validateValue(LuaType::Number, value.second, key, prefix);
				ret.basePort = value.second.as<world_id_t>();
			}
			else if (key == "port") {
				hasPort = true;
				config.validateValue(LuaType::Number, value.second, key, prefix);
				ret.basePort = value.second.as<port_t>();
			}
			else if (key == "gm_chat_by_default") {
				if (config.validateValue(LuaType::Bool, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.defaultGmChatMode = value.second.as<bool>();
			}
			else if (key == "channels") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.maxChannels = value.second.as<channel_id_t>();
			}
			else if (key == "ribbon") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.ribbon = value.second.as<int8_t>();
			}
			else if (key == "max_stats") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.maxStats = value.second.as<stat_t>();
			}
			else if (key == "max_multi_level") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.maxMultiLevel = value.second.as<player_level_t>();
			}
			else if (key == "event_message") {
				if (config.validateValue(LuaType::String, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.eventMessage = value.second.as<string_t>();
			}
			else if (key == "scrolling_header") {
				if (config.validateValue(LuaType::String, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.scrollingHeader = value.second.as<string_t>();
			}
			else if (key == "max_player_load") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.maxPlayerLoad = value.second.as<int32_t>();
			}
			else if (key == "max_characters") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.maxChars = value.second.as<int32_t>();
			}
			else if (key == "default_characters") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.defaultChars = value.second.as<int32_t>();
			}
			else if (key == "default_storage") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.defaultStorageSlots = value.second.as<storage_slot_t>();
			}
			else if (key == "fame_time") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.fameTime = value.second.as<seconds_t>();
			}
			else if (key == "fame_reset_time") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.fameResetTime = value.second.as<seconds_t>();
			}
			else if (key == "map_unload_time") {
				if (config.validateValue(LuaType::Number, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.mapUnloadTime = value.second.as<seconds_t>();
			}
			else if (key == "rates") {
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.rates = value.second.into<RatesConfig>(config, prefix + "." + key);
			}
			else if (key == "pianus") {
				hasPianus = true;
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.pianus = value.second.into<MajorBossConfig>(config, prefix + "." + key);
			}
			else if (key == "papulatus") {
				hasPapulatus = true;
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.papulatus = value.second.into<MajorBossConfig>(config, prefix + "." + key);
			}
			else if (key == "zakum") {
				hasZakum = true;
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.zakum = value.second.into<MajorBossConfig>(config, prefix + "." + key);
			}
			else if (key == "horntail") {
				hasHorntail = true;
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.horntail = value.second.into<MajorBossConfig>(config, prefix + "." + key);
			}
			else if (key == "pinkbean") {
				hasPinkbean = true;
				if (config.validateValue(LuaType::Table, value.second, key, prefix, true) == LuaType::Nil) continue;
				ret.pinkbean = value.second.into<MajorBossConfig>(config, prefix + "." + key);
			}
		}

		config.required(hasName, "name", prefix);
		config.required(hasId, "id", prefix);
		config.required(hasPort, "port", prefix);

		if (hasPianus) expandMajorBoss(ret, ret.pianus);
		if (hasPapulatus) expandMajorBoss(ret, ret.papulatus);
		if (hasZakum) expandMajorBoss(ret, ret.zakum);
		if (hasHorntail) expandMajorBoss(ret, ret.horntail);
		if (hasPinkbean) expandMajorBoss(ret, ret.pinkbean);

		return ret;
	}
};

template <>
struct PacketSerialize<WorldConfig> {
	auto read(PacketReader &reader) -> WorldConfig {
		WorldConfig ret;
		ret.id = reader.get<world_id_t>();
		ret.basePort = reader.get<port_t>();
		ret.defaultGmChatMode = reader.get<bool>();
		ret.ribbon = reader.get<int8_t>();
		ret.maxMultiLevel = reader.get<player_level_t>();
		ret.defaultStorageSlots = reader.get<storage_slot_t>();
		ret.maxStats = reader.get<stat_t>();
		ret.defaultChars = reader.get<int32_t>();
		ret.maxChars = reader.get<int32_t>();
		ret.maxPlayerLoad = reader.get<int32_t>();
		ret.fameTime = reader.get<seconds_t>();
		ret.fameResetTime = reader.get<seconds_t>();
		ret.mapUnloadTime = reader.get<seconds_t>();
		ret.maxChannels = reader.get<channel_id_t>();
		ret.eventMessage = reader.get<string_t>();
		ret.scrollingHeader = reader.get<string_t>();
		ret.name = reader.get<string_t>();
		ret.rates = reader.get<RatesConfig>();
		ret.pianus = reader.get<MajorBossConfig>();
		ret.papulatus = reader.get<MajorBossConfig>();
		ret.zakum = reader.get<MajorBossConfig>();
		ret.horntail = reader.get<MajorBossConfig>();
		ret.pinkbean = reader.get<MajorBossConfig>();
		return ret;
	}
	auto write(PacketBuilder &builder, const WorldConfig &obj) -> void {
		builder.add<world_id_t>(obj.id);
		builder.add<port_t>(obj.basePort);
		builder.add<bool>(obj.defaultGmChatMode);
		builder.add<int8_t>(obj.ribbon);
		builder.add<player_level_t>(obj.maxMultiLevel);
		builder.add<storage_slot_t>(obj.defaultStorageSlots);
		builder.add<stat_t>(obj.maxStats);
		builder.add<int32_t>(obj.defaultChars);
		builder.add<int32_t>(obj.maxChars);
		builder.add<int32_t>(obj.maxPlayerLoad);
		builder.add<seconds_t>(obj.fameTime);
		builder.add<seconds_t>(obj.fameResetTime);
		builder.add<seconds_t>(obj.mapUnloadTime);
		builder.add<channel_id_t>(obj.maxChannels);
		builder.add<string_t>(obj.eventMessage);
		builder.add<string_t>(obj.scrollingHeader);
		builder.add<string_t>(obj.name);
		builder.add<RatesConfig>(obj.rates);
		builder.add<MajorBossConfig>(obj.pianus);
		builder.add<MajorBossConfig>(obj.papulatus);
		builder.add<MajorBossConfig>(obj.zakum);
		builder.add<MajorBossConfig>(obj.horntail);
		builder.add<MajorBossConfig>(obj.pinkbean);
	}
};