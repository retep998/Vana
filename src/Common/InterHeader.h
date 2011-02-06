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
#pragma once

// Inter-server communication does not separate send and recv because the server does both
enum AnyConnection {
	IMSG_PASSWORD = 0xff
};

enum LoginWorld {
	IMSG_WORLD_CONNECT = 0x1000,
	IMSG_REGISTER_CHANNEL,
	IMSG_UPDATE_CHANNEL_POP,
	IMSG_REMOVE_CHANNEL,
	IMSG_NEW_PLAYER,
	IMSG_GUILD_OPERATION,
	IMSG_REGISTER_CASH_SERVER,
	IMSG_REMOVE_CASH_SERVER
};

enum LoginChannel {
	IMSG_LOGIN_CHANNEL_CONNECT = 0x2000, // Get world server info
	IMSG_CALCULATE_RANKING
};

enum WorldChannel {
	IMSG_CHANNEL_CONNECT = 0x3000,
	IMSG_TO_PLAYERS, // Pass the content of the packet to player of all channel servers
	IMSG_FIND, // "/find" command
	IMSG_WHISPER,
	IMSG_SCROLLING_HEADER,
	IMSG_FORWARD_TO,
	IMSG_GROUP_CHAT,
	IMSG_SET_RATES,
	IMSG_TO_LOGIN, // Channel servers send this to ask the world server to send something to login server
	IMSG_TO_WORLDS, // For sending a packet to all worlds via the loginserver
	IMSG_TO_CHANNELS, // For sending a packet from a channel to all channels via the WorldServer
	IMSG_TO_CASH_SERVER, // For sending a packet from a channel to the cash server
	IMSG_REFRESH_DATA, // For reloading MCDB
	IMSG_SYNC,
	IMSG_LOAD_PLAYER_NPC
};

enum WorldCashServer {
	IMSG_CASH_SERVER_CONNECT = 0x4000,
	IMSG_CONNECT_PLAYER,
	IMSG_REMOVE_PLAYER,
	IMSG_UPDATE_COUPON,
	IMSG_RECALCULATE_BEST_ITEMS
};

enum ConnectionTypes {
	InterLoginServer,
	InterWorldServer,
	InterChannelServer,
	InterCashServer,
	InterMtsServer
};