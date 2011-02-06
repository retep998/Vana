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
#ifndef LOGINSERVERCONNECTPACKET_H
#define LOGINSERVERCONNECTPACKET_H

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class LoginServerConnection;

namespace LoginServerConnectPacket {
	void registerChannel(LoginServerConnection *player, int32_t channel, uint32_t ip, const vector<vector<uint32_t> > &extIp, int16_t port);
	void updateChannelPop(LoginServerConnection *player, int32_t channel, int32_t population);
	void removeChannel(LoginServerConnection *player, int32_t channel);
};

#endif
