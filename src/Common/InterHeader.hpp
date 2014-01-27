/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Types.hpp"

// Inter-server communication does not separate send and recv because the server does both
enum AnyConnection : header_t {
	IMSG_PASSWORD = 0xff,
	IMSG_REHASH_CONFIG,
	IMSG_TO_LOGIN,
	IMSG_TO_WORLD,
	IMSG_TO_CHANNELS,
	IMSG_TO_WORLDS,
	IMSG_REFRESH_DATA, // For reloading MCDB
	IMSG_SYNC,
};

enum LoginWorld : header_t {
	IMSG_WORLD_CONNECT = 0x1000,
	IMSG_REGISTER_CHANNEL,
	IMSG_UPDATE_CHANNEL_POP,
	IMSG_REMOVE_CHANNEL,
	IMSG_NEW_PLAYER,
};

enum LoginChannel : header_t {
	IMSG_LOGIN_CHANNEL_CONNECT = 0x2000, // Get world server info
	IMSG_CALCULATE_RANKING,
};

enum WorldChannel : header_t {
	IMSG_CHANNEL_CONNECT = 0x3000,
	IMSG_TO_PLAYER,
	IMSG_TO_PLAYERS, // Pass the content of the packet to player of all channel servers
	IMSG_GROUP_CHAT,
	IMSG_FIND, // "/find" command
	IMSG_WHISPER,
};