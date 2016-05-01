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
#include "SaltTransformationConfig.hpp"
#include "Common/BitUtilities.hpp"
#include "Common/SaltLeftoverPolicy.hpp"
#include "Common/Types.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace Vana {

SaltTransformationConfig::SaltTransformationConfig(SaltModifyPolicy policy, vector_t<LuaVariant> args) :
	m_policy{policy},
	m_args{args}
{
}

auto SaltTransformationConfig::validateArgs(SaltModifyPolicy policy, const vector_t<LuaVariant> &args) -> ValidityResult {
	switch (policy) {
		case SaltModifyPolicy::XorByteCipher: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.isAny({ LuaType::Number, LuaType::String })) return ValidityResult::Invalid;
			if (arg.is(LuaType::String)) {
				const auto &str = arg.as<string_t>();
				if (str.size() != 1) return ValidityResult::Invalid;
			}
			break;
		}
		case SaltModifyPolicy::XorCipher: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.isAny({ LuaType::Table, LuaType::String })) return ValidityResult::Invalid;
			if (arg.is(LuaType::Table)) {
				auto props = arg.as<hash_map_t<LuaVariant, LuaVariant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
					if (!kvp.second.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
				}
			}
			break;
		}
		case SaltModifyPolicy::BitRotateCipher:
		case SaltModifyPolicy::OverallBitRotateCipher:
		case SaltModifyPolicy::ByteRotateCipher: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.is(LuaType::Number)) return ValidityResult::Invalid;
			break;
		}
		case SaltModifyPolicy::BitReverseCipher:
		case SaltModifyPolicy::ReverseCipher: {
			// Intentionally blank
			break;
		}
		case SaltModifyPolicy::OverwriteCipher: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.is(LuaType::Table)) return ValidityResult::Invalid;
			auto &map = arg.as<hash_map_t<LuaVariant, LuaVariant>>();
			for (const auto &kvp : map) {
				if (!kvp.first.is(LuaType::Number)) {
					return ValidityResult::Invalid;
				}
				if (!kvp.second.isAny({ LuaType::Number, LuaType::String })) {
					return ValidityResult::Invalid;
				}
				if (kvp.second.is(LuaType::String)) {
					if (kvp.second.as<string_t>().size() != 1) {
						return ValidityResult::Invalid;
					}
				}
			}
			break;
		}
		case SaltModifyPolicy::AddCipher:
		case SaltModifyPolicy::SubtractCipher:
		case SaltModifyPolicy::MultiplyCipher:
		case SaltModifyPolicy::DivideCipher: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.is(LuaType::Number)) return ValidityResult::Invalid;
			break;
		}
		case SaltModifyPolicy::Append: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.isAny({ LuaType::String, LuaType::Table })) return ValidityResult::Invalid;
			if (arg.is(LuaType::Table)) {
				auto props = arg.as<hash_map_t<LuaVariant, LuaVariant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
					if (!kvp.second.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
				}
			}
			break;
		}
		case SaltModifyPolicy::Prepend: {
			if (args.size() != 1) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.isAny({ LuaType::String, LuaType::Table })) return ValidityResult::Invalid;
			if (arg.is(LuaType::Table)) {
				auto props = arg.as<hash_map_t<LuaVariant, LuaVariant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
					if (!kvp.second.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
				}
			}
			break;
		}
		case SaltModifyPolicy::Intersperse: {
			if (args.size() <= 2 || args.size() >= 5) return ValidityResult::Invalid;
			const auto &arg = args[0];
			if (!arg.isAny({ LuaType::String, LuaType::Table })) return ValidityResult::Invalid;
			if (arg.is(LuaType::Table)) {
				auto props = arg.as<hash_map_t<LuaVariant, LuaVariant>>();
				for (const auto &kvp : props) {
					if (!kvp.first.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
					if (!kvp.second.is(LuaType::Number)) {
						return ValidityResult::Invalid;
					}
				}
			}

			if (!args[1].is(LuaType::Number)) return ValidityResult::Invalid;
			if (!args[2].is(LuaType::Number)) return ValidityResult::Invalid;
			if (args.size() == 4) {
				if (!args[3].is(LuaType::Number)) return ValidityResult::Invalid;
			}
			break;
		}
		default: throw NotImplementedException{"SaltModifyPolicy"};
	}
	return ValidityResult::Valid;
}

auto SaltTransformationConfig::apply(string_t input) const -> string_t {
	string_t ret;
	switch (m_policy) {
		case SaltModifyPolicy::XorCipher: {
			const auto &arg = m_args[0];
			string_t intersperse;
			if (arg.is(LuaType::String)) {
				intersperse = m_args[0].as<string_t>();
			}
			else {
				const auto &bytes = arg.as<vector_t<uint8_t>>();
				intersperse = string_t{std::begin(bytes), std::end(bytes)};
			}

			size_t minSize = std::min(intersperse.size(), input.size());
			ret = input;
			for (size_t i = 0; i < minSize; i++) {
				ret[i] = ret[i] ^ intersperse[i];
			}
			break;
		}
		case SaltModifyPolicy::BitRotateCipher: {
			int32_t value = m_args[0].as<int32_t>();
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				ret[i] = BitUtilities::rotateRight<uint8_t>(ret[i], value);
			}
			break;
		}
		case SaltModifyPolicy::OverallBitRotateCipher: {
			int32_t value = m_args[0].as<int32_t>();
			ret = input;
			if (ret.size() == 1) {
				ret[0] = BitUtilities::rotateRight<uint8_t>(ret[0], value);
			}
			else {
				bool bit = true;
				bool tempBit = false;
				size_t end = ret.size() - 1;
				size_t first = end;
				uint8_t bitValue = 0x01;
				uint8_t bitMatch = 0x80;

				for (int32_t shift = 0; shift < value; shift++) {
					uint8_t saved = ret[first];
					for (size_t idx = end; /* Intentionally blank */; idx--) {
						tempBit = (ret[idx] & bitMatch) == bitMatch;
						ret[idx] <<= 1;
						ret[idx] |= static_cast<uint8_t>(bit ? bitValue : 0);
						bit = tempBit;
						if (idx == 0) break;
					}

					tempBit = (saved & bitMatch) == bitMatch;
					saved <<= 1;
					saved |= static_cast<uint8_t>(bit ? bitValue : 0);
					ret[first] = saved;
					bit = tempBit;
				}
			}
			break;
		}
		case SaltModifyPolicy::ByteRotateCipher: {
			size_t amount = m_args[0].as<uint32_t>();
			ret = input;
			auto newTop = std::begin(ret);
			std::advance(newTop, amount);
			std::rotate(std::begin(ret), newTop, std::end(ret));
			break;
		}
		case SaltModifyPolicy::BitReverseCipher: {
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				ret[i] = BitUtilities::reverse<uint8_t>(ret[i]);
			}
			break;
		}
		case SaltModifyPolicy::ReverseCipher: {
			ret = input;
			std::reverse(std::begin(ret), std::end(ret));
			break;
		}
		case SaltModifyPolicy::OverwriteCipher: {
			const auto &arg = m_args[0].as<hash_map_t<uint32_t, LuaVariant>>();
			ret = input;
			for (const auto &replace : arg) {
				uint8_t value;
				if (replace.second.is(LuaType::Number)) {
					value = replace.second.as<uint8_t>();
				}
				else {
					value = replace.second.as<string_t>()[0];
				}
				ret[replace.first] = value;
			}
			break;
		}
		case SaltModifyPolicy::XorByteCipher:
		case SaltModifyPolicy::AddCipher:
		case SaltModifyPolicy::SubtractCipher:
		case SaltModifyPolicy::MultiplyCipher:
		case SaltModifyPolicy::DivideCipher: {
			uint8_t value = 0;
			if (m_policy == SaltModifyPolicy::XorByteCipher) {
				const auto &arg = m_args[0];
				if (arg.is(LuaType::Number)) {
					value = m_args[0].as<uint8_t>();
				}
				else {
					value = m_args[0].as<string_t>()[0];
				}
			}
			else {
				value = m_args[0].as<uint8_t>();	
			}
			ret = input;
			for (size_t i = 0; i < ret.size(); i++) {
				switch (m_policy) {
					case SaltModifyPolicy::XorByteCipher: ret[i] ^= value; break;
					case SaltModifyPolicy::AddCipher: ret[i] += value; break;
					case SaltModifyPolicy::SubtractCipher: ret[i] -= value; break;
					case SaltModifyPolicy::MultiplyCipher: ret[i] *= value; break;
					case SaltModifyPolicy::DivideCipher: ret[i] /= value; break;
					default: throw NotImplementedException{"SaltModifyPolicy"};
				}
			}
			break;
		}
		case SaltModifyPolicy::Append: {
			const auto &arg = m_args[0];
			if (arg.is(LuaType::String)) {
				ret = input + arg.as<string_t>();
			}
			else {
				const auto &bytes = arg.as<vector_t<uint8_t>>();
				ret = input + string_t{std::begin(bytes), std::end(bytes)};
			}
			break;
		}
		case SaltModifyPolicy::Prepend: {
			const auto &arg = m_args[0];
			if (arg.is(LuaType::String)) {
				ret = arg.as<string_t>() + input;
			}
			else {
				const auto &bytes = arg.as<vector_t<uint8_t>>();
				ret = string_t{std::begin(bytes), std::end(bytes)} + input;
			}
			break;
		}
		case SaltModifyPolicy::Intersperse: {
			const auto &arg = m_args[0];
			string_t intersperse;
			if (arg.is(LuaType::String)) {
				intersperse = m_args[0].as<string_t>();
			}
			else {
				const auto &bytes = arg.as<vector_t<uint8_t>>();
				intersperse = string_t{std::begin(bytes), std::end(bytes)};
			}
			size_t numberOfChars = m_args[1].as<uint32_t>();
			size_t offsetChars = m_args[2].as<uint32_t>();
			SaltLeftoverPolicy policy = SaltLeftoverPolicy::Discard;
			if (m_args.size() == 4) {
				policy = static_cast<SaltLeftoverPolicy>(m_args[3].as<int32_t>());
			}

			ret = "";
			string_t apply = input;
			bool startProcessing = false;
			size_t interspersedPos = 0;
			do {
				size_t i = 0;
				for (/* Intentionally blank */; i < apply.size(); i++) {
					if (interspersedPos == intersperse.size()) {
						// No more salt to apply
						break;
					}

					if (i == offsetChars) {
						startProcessing = true;
					}
					else if (startProcessing) {
						if (i % numberOfChars == 0) {
							ret += intersperse[interspersedPos++];
						}
					}
					ret += apply[i];
				}

				if (policy == SaltLeftoverPolicy::Rollover) {
					if (interspersedPos < intersperse.size()) {
						intersperse = intersperse.substr(interspersedPos);
						apply = ret;
						ret = "";
						interspersedPos = 0;
						continue;
					}
				}

				if (i < apply.size()) {
					ret += apply.substr(i);
				}

				break;
			}
			while (true);

			if (interspersedPos < intersperse.size()) {
				if (policy == SaltLeftoverPolicy::Append) {
					ret = ret + intersperse.substr(interspersedPos);
				}
				else if (policy == SaltLeftoverPolicy::Prepend) {
					ret = intersperse.substr(interspersedPos) + ret;
				}
			}
			break;
		}
		default: throw NotImplementedException{"SaltModifyPolicy"};
	}
	return ret;
}

}