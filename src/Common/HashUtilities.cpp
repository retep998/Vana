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
#include "HashUtilities.hpp"
#include "Common/SaltConfig.hpp"
#include "Common/SaltSizeConfig.hpp"
#include <botan/filters.h>
#include <botan/pipe.h>

namespace Vana {
namespace HashUtilities {

auto hashPassword(const string_t &password) -> string_t {
	Botan::Pipe pipe{
		new Botan::Chain{
			new Botan::Hash_Filter{"SHA-512"},
			new Botan::Hex_Encoder{}}};

	pipe.process_msg(password);
	return pipe.read_all_as_string();
}

auto hashPassword(const string_t &password, const string_t &rawSalt, const SaltConfig &conf) -> string_t {
	return hashPassword(saltPassword(password, rawSalt, conf));
}

auto saltPassword(const string_t &password, const string_t &rawSalt, const SaltConfig &conf) -> string_t {
	string_t finalizedSalt = rawSalt;
	for (const auto &policy : conf.modifyPolicies) {
		finalizedSalt = policy.apply(finalizedSalt);
	}

	return conf.policy.apply(password, finalizedSalt);
}

auto generateSalt(const SaltSizeConfig &conf) -> string_t {
	int32_t length = 0;
	switch (conf.policy) {
		case SaltSizePolicy::Static:
			length = conf.size;
			break;
		case SaltSizePolicy::Random:
			length = Randomizer::rand(conf.max, conf.min);
			break;
	}

	// Explicitly using () constructor style here because {} produces different behavior
	string_t salt(length, 0);
	for (int32_t i = 0; i < length; i++) {
		salt[i] = Randomizer::rand<uint8_t>(255, 0);
	}
	return salt;
}

}
}