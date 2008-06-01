/*
Copyright (C) 2008 Vana Development Team

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
#ifndef LOGIN_H
#define LOGIN_H

#include <string>

using std::string;
using std::ostringstream;

class PlayerLogin;
class ReadPacket;

namespace Login {
	void loginUser(PlayerLogin* player, ReadPacket *packet);
	void setGender(PlayerLogin* player, ReadPacket *packet);
	void handleLogin(PlayerLogin* player, ReadPacket *packet);
	void registerPIN(PlayerLogin* player, ReadPacket *packet);
	void loginBack(PlayerLogin* player);
	void checkPin(PlayerLogin* player, ReadPacket *packet);
	string hashPassword(const string &password, const string &salt);
};

#endif