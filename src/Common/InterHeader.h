/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef INTERHEADER_H
#define INTERHEADER_H

//Inter-server communication does not separate send and recv because the server does both
#define INTER_PASSWORD 0xff

// Login <--> World
#define INTER_WORLD_CONNECT 0x1000
#define INTER_REGISTER_CHANNEL 0x1001
#define INTER_NEW_PLAYER 0x1002
#define INTER_UPDATE_CHANNEL_POP 0x1003
#define INTER_REMOVE_CHANNEL 0x1004

// World <--> Channel
#define INTER_LOGIN_CHANNEL_CONNECT 0x2000 // Login <--> Channel to get World server info
#define INTER_CHANNEL_CONNECT 0x2001
#define INTER_PLAYER_CHANGE_CHANNEL 0x2002
#define INTER_TO_PLAYERS 0x2003 // Pass the content of the packet to player of all channel servers
#define INTER_REGISTER_PLAYER 0x2004
#define INTER_REMOVE_PLAYER 0x2005
#define INTER_FIND 0x2006 // "/find" command
#define INTER_WHISPER 0x2007
#define INTER_SCROLLING_HEADER 0x2008
#define INTER_NEW_CONNECTABLE 0x2009
#define INTER_PARTY_OPERATION 0x2010
#define INTER_PARTY_SYNC 0x2011
#define INTER_UPDATE_LEVEL 0x2012
#define INTER_UPDATE_JOB 0x2013
#define INTER_UPDATE_MAP 0x2014
#define INTER_FORWARD_TO 0x2015
#define INTER_GROUP_CHAT 0x2016
#define INTER_SET_RATES 0x2017
#define INTER_TO_LOGIN 0x2018 // Channel servers send this to ask the world server to send something to login server
#define INTER_CALCULATE_RANKING 0x2019
#define INTER_TRANSFER_PLAYER_PACKET 0x2020
#define INTER_TRANSFER_PLAYER_PACKET_DISCONNECT 0x2021
#define INTER_TO_WORLDS 0x2022 // For sending a packet to all worlds via the loginserver
#define INTER_TO_CHANNELS 0x2023 // For sending a packet from a channel to all channels via the WorldServer
#define INTER_REFRESH_DATA 0x2024 // For reloading MCDB
#define INTER_GUILD_OPERATION 0x2025
#define INTER_BBS 0x2026
#define INTER_ALLIANCE 0x2027

enum {
	InterLoginServer,
	InterWorldServer,
	InterChannelServer
};

#endif
