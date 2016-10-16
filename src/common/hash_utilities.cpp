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
#include "hash_utilities.hpp"
#include "common/config/salt.hpp"
#include "common/config/salt_size.hpp"
#include <botan/filters.h>
#include <botan/pipe.h>

namespace vana {
namespace hash_utilities {

auto hash_password(const string &password) -> string {
	Botan::Pipe pipe{
		new Botan::Chain{
			new Botan::Hash_Filter{"SHA-512"},
			new Botan::Hex_Encoder{}}};

	pipe.process_msg(password);
	return pipe.read_all_as_string();
}

auto hash_password(const string &password, const string &raw_salt, const config::salt &conf) -> string {
	return hash_password(salt_password(password, raw_salt, conf));
}

auto salt_password(const string &password, const string &raw_salt, const config::salt &conf) -> string {
	string finalized_salt = raw_salt;
	for (const auto &policy : conf.modify_policies) {
		finalized_salt = policy.apply(finalized_salt);
	}

	return conf.policy.apply(password, finalized_salt);
}

auto generate_salt(const config::salt_size &conf) -> string {
	int32_t length = 0;
	switch (conf.policy) {
		case salt_size_policy::fixed:
			length = conf.size;
			break;
		case salt_size_policy::random:
			length = vana::util::randomizer::rand(conf.max, conf.min);
			break;
	}

	// Explicitly using () constructor style here because {} produces different behavior
	string salt(length, 0);
	for (int32_t i = 0; i < length; i++) {
		salt[i] = vana::util::randomizer::rand<uint8_t>(255, 0);
	}
	return salt;
}

}
}