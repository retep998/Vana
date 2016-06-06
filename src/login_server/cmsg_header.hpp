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

#define CMSG_AUTHENTICATION 0x01
#define CMSG_WORLD_LIST_REFRESH 0x04 // Click back after select channel
#define CMSG_PLAYER_LIST 0x05
#define CMSG_WORLD_STATUS 0x06
#define CMSG_ACCOUNT_GENDER 0x08
#define CMSG_PIN 0x09
#define CMSG_REGISTER_PIN 0x0a
#define CMSG_WORLD_LIST 0x0b
#define CMSG_PLAYER_GLOBAL_LIST 0x0d
#define CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT 0x0e
#define CMSG_REQUEST_NAME_CHANGE 0x10 // Are these request packets login or channel?
#define CMSG_REQUEST_CHARACTER_TRANSFER 0x12
#define CMSG_CHANNEL_CONNECT 0x13
#define CMSG_PLAYER_NAME_CHECK 0x15
#define CMSG_PLAYER_CREATE 0x16
#define CMSG_PLAYER_DELETE 0x18
#define CMSG_CLIENT_ERROR 0x1a
#define CMSG_LOGIN_RETURN 0x1d
#define CMSG_CLIENT_STARTED 0x22