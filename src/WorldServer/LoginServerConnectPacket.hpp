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

#include "Common/ExternalIp.hpp"
#include "Common/Ip.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	namespace WorldServer {
		namespace Packets {
			PACKET(registerChannel, channel_id_t channel, const Ip &channelIp, const IpMatrix &extIp, port_t port);
			PACKET(updateChannelPop, channel_id_t channel, int32_t population);
			PACKET(removeChannel, channel_id_t channel);
		}
	}
}