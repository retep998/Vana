/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "MiscUtilities.h"
#include "VanaConstants.h"
#include <filters.h>
#include <hex.h>
#include <sha.h>

string MiscUtilities::hashPassword(const string &password, const string &salt) {
	string salted = salt + password;
	string digest;

	CryptoPP::SHA512 hash;

	CryptoPP::StringSource(salted, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest))));

	return digest;
}

bool MiscUtilities::isBossChannel(const vector<int8_t> &vec, int8_t channelid) {
	for (vector<int8_t>::const_iterator iter = vec.begin(); iter != vec.end(); iter++) {
		if (*iter == channelid) {
			return true;
		}
	}
	return false;
}