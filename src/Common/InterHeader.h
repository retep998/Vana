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
#ifndef INTERHEADER_H
#define INTERHEADER_H

//Inter-server communication does not separate send and recv because the server does both
#define INTER_PASSWORD 0xff

// Login <--> World
#define INTER_WORLD_CONNECT 0x1000
#define INTER_REGISTER_CHANNEL 0x1001

// World <--> Channel
#define INTER_LOGIN_CHANNEL_CONNECT 0x2000 // Login <--> Channel to get World server info
#define INTER_CHANNEL_CONNECT 0x2001
#define INTER_CHANNEL_NUM 0x2002

enum {
	INTER_LOGIN_SERVER,
	INTER_WORLD_SERVER,
	INTER_CHANNEL_SERVER
};

#endif