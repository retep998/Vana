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
// CryptoPP
#include <filters.h>
#include <hex.h>
#include <sha.h>

string MiscUtilities::hashPassword(const string &password, const string &salt) {
	string salted = salt + password;
	string digest;

	if (salt.length() == 5) {
		// LEGACY
		// TODO: EVENTUALLY REMOVE
		CryptoPP::SHA1 hash;

		CryptoPP::StringSource(salted, true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
	}
	else {
		CryptoPP::SHA512 hash;

		CryptoPP::StringSource(salted, true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(digest))));
	}

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

string MiscUtilities::getBanReason(int8_t reason) {
	switch (reason) {
		case 0x01: return "hacking.";
		case 0x02: return "using macro/auto-keyboard.";
		case 0x03: return "illicit promotion or advertising.";
		case 0x04: return "harassment.";
		case 0x05: return "using profane language.";
		case 0x06: return "scamming.";
		case 0x07: return "misconduct.";
		case 0x08: return "illegal cash transaction.";
		case 0x09: return "illegal charging/funding.";
		case 0x0A: return "temporary request.";
		case 0x0B: return "impersonating GM.";
		case 0x0C: return "using illegal programs or violating the game policy.";
		case 0x0D: return "one of cursing, scamming, or illegal trading via Megaphones.";
		default: return "an unknown reason.";
	}
}