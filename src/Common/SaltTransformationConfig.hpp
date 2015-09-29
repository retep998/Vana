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
#include "Common/LuaVariant.hpp"
#include "Common/SaltModifyPolicy.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	class LuaVariant;

	class SaltTransformationConfig {
	public:
		SaltTransformationConfig(SaltModifyPolicy policy, vector_t<LuaVariant> args);

		auto static validateArgs(SaltModifyPolicy policy, const vector_t<LuaVariant> &args) -> ValidityResult;
		auto apply(string_t input) const -> string_t;
	private:
		SaltTransformationConfig() = default;
		friend struct LuaVariantInto<SaltTransformationConfig>;

		SaltModifyPolicy m_policy = SaltModifyPolicy::None;
		vector_t<LuaVariant> m_args;
	};

	template <>
	struct LuaVariantInto<SaltTransformationConfig> {
		auto transform(LuaEnvironment &config, const LuaVariant &src, const string_t &prefix) -> SaltTransformationConfig {
			SaltTransformationConfig ret;

			config.validateObject(LuaType::Table, src, prefix);

			auto map = src.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasType = false;
			bool hasArgs = false;
			SaltModifyPolicy saltPolicyData;
			LuaVariant args;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "type") {
					hasType = true;
					config.validateValue(LuaType::Number, kvp.second, key, prefix);
					saltPolicyData = static_cast<SaltModifyPolicy>(kvp.second.as<int32_t>());
				}
				else if (key == "args") {
					hasArgs = true;
					config.validateValue(LuaType::Table, kvp.second, key, prefix);
					args = kvp.second;
				}
			}

			config.required(hasType, "type", prefix);
			config.required(hasArgs, "args", prefix);

			ret.m_policy = saltPolicyData;
			ret.m_args = args.as<vector_t<LuaVariant>>();

			if (SaltTransformationConfig::validateArgs(ret.m_policy, ret.m_args) == ValidityResult::Invalid) {
				config.error("Arguments for salt transformation " + prefix + " are invalid. Please consult the documentation.");
			}

			return ret;
		}
	};
}