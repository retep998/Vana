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
#include "MiscUtilities.hpp"
#include <botan/pipe.h>
#include <botan/filters.h>

auto MiscUtilities::hashPassword(const string_t &password, const string_t &salt) -> string_t {
	string_t salted = salt + password;
	Botan::Pipe pipe(
		new Botan::Chain(
			new Botan::Hash_Filter("SHA-512"),
			new Botan::Hex_Encoder()));

	pipe.process_msg(salted);
	return pipe.read_all_as_string();
}

auto MiscUtilities::generateSalt(size_t length) -> string_t {
	string_t salt(length, 0);
	for (size_t i = 0; i < length; i++) {
		salt[i] = Randomizer::rand<uint8_t>(126, 33);
	}
	return salt;
}