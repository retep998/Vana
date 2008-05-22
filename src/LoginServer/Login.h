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

#include "LoginPacket.h"
#include "MySQLM.h"
#include "BufferUtilities.h"
#include <stdio.h>
#include "PlayerLogin.h"
#include "TimeUtilities.h"
#include "LoginServer.h"

class PlayerLogin;

namespace Login {
	void loginUser(PlayerLogin* player, unsigned char* packet);
	void setGender(PlayerLogin* player, unsigned char* packet);
	void handleLogin(PlayerLogin* player, unsigned char* packet);
	void registerPIN(PlayerLogin* player, unsigned char* packet);
	void loginBack(PlayerLogin* player);
	void checkPin(PlayerLogin* player, unsigned char* packet);
	char * hashPassword(const char *password, const char *salt);
};

#endif