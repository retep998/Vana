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
#include "PasswordTransformationConfig.hpp"
#include "Common/SaltLeftoverPolicy.hpp"
#include "Common/Types.hpp"
#include <cstdlib>
#include <string>
#include <vector>

namespace Vana {

PasswordTransformationConfig::PasswordTransformationConfig(SaltPolicy policy, vector_t<LuaVariant> args) :
	m_policy{policy},
	m_args{args}
{
}

auto PasswordTransformationConfig::validateArgs(SaltPolicy policy, const vector_t<LuaVariant> &args) -> ValidityResult {
	switch (policy) {
		case SaltPolicy::Append:
		case SaltPolicy::Prepend: {
			if (args.size() > 0) return ValidityResult::Invalid;
			break;
		}
		case SaltPolicy::Intersperse: {
			if (args.size() <= 1) return ValidityResult::Invalid;
			if (args.size() >= 4) return ValidityResult::Invalid;
			if (!args[0].is(LuaType::Number)) return ValidityResult::Invalid;
			if (!args[1].is(LuaType::Number)) return ValidityResult::Invalid;
			if (args.size() == 3) {
				if (!args[2].is(LuaType::Number)) return ValidityResult::Invalid;
			}
			break;
		}
		case SaltPolicy::ChopDistribute:
		case SaltPolicy::ChopCrossDistribute: {
			if (args.size() == 0) return ValidityResult::Invalid;
			if (args.size() >= 3) return ValidityResult::Invalid;
			for (const auto &arg : args) {
				if (!arg.is(LuaType::Number)) return ValidityResult::Invalid;
			}
			break;
		}
		default:
			throw NotImplementedException{"SaltPolicy"};
	}
	return ValidityResult::Valid;
}

auto PasswordTransformationConfig::apply(string_t input, string_t salt) const -> string_t {
	string_t ret;
	switch (m_policy) {
		case SaltPolicy::Append:
			ret = input + salt;
			break;
		case SaltPolicy::Prepend:
			ret = salt + input;
			break;
		case SaltPolicy::Intersperse: {
			string_t intersperse = salt;
			size_t numberOfChars = m_args[0].as<uint32_t>();
			size_t offsetChars = m_args[1].as<uint32_t>();
			SaltLeftoverPolicy policy = SaltLeftoverPolicy::Discard;
			if (m_args.size() == 3) {
				policy = static_cast<SaltLeftoverPolicy>(m_args[2].as<int32_t>());
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
		case SaltPolicy::ChopDistribute:
		case SaltPolicy::ChopCrossDistribute: {
			size_t splitPointOrNumberOfBeginningChars = m_args[0].as<uint32_t>();
			bool hasSplitPoint = true;
			size_t numberOfEndChars = 0;
			if (m_args.size() == 2) {
				hasSplitPoint = false;
				numberOfEndChars = m_args[1].as<uint32_t>();
			}

			string_t beginChop = salt.substr(0, splitPointOrNumberOfBeginningChars);
			string_t endChop = salt.substr(
				hasSplitPoint ?
					splitPointOrNumberOfBeginningChars :
					salt.size() - numberOfEndChars,
				hasSplitPoint ?
					string_t::npos :
					numberOfEndChars);

			if (m_policy == SaltPolicy::ChopDistribute) {
				ret = beginChop + input + endChop;
			}
			else {
				ret = endChop + input + beginChop;
			}
			break;
		}
		default:
			throw NotImplementedException{"SaltPolicy"};
	}
	return ret;
}

}