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
#pragma once

#include "common/types_temp.hpp"

namespace vana {

// Inter-server communication does not separate send and recv because the server does both
enum any_connection : packet_header {
	IMSG_PASSWORD = 0xff,
	IMSG_REHASH_CONFIG,
	IMSG_TO_LOGIN,
	IMSG_TO_WORLD,
	IMSG_TO_WORLD_LIST,
	IMSG_TO_ALL_WORLDS,
	IMSG_TO_CHANNEL,
	IMSG_TO_CHANNEL_LIST,
	IMSG_TO_ALL_CHANNELS,
	IMSG_REFRESH_DATA,
	IMSG_SYNC,
};

enum login_world : packet_header {
	IMSG_WORLD_CONNECT = 0x1000,
	IMSG_REGISTER_CHANNEL,
	IMSG_UPDATE_CHANNEL_POP,
	IMSG_REMOVE_CHANNEL,
};

enum login_channel : packet_header {
	IMSG_LOGIN_CHANNEL_CONNECT = 0x2000,
	IMSG_CALCULATE_RANKING,
};

enum world_channel : packet_header {
	IMSG_CHANNEL_CONNECT = 0x3000,
	IMSG_TO_PLAYER,
	IMSG_TO_PLAYER_LIST,
	IMSG_TO_ALL_PLAYERS,
};

}