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
#include "AuthenticationPacket.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketBuilder.hpp"
#include <algorithm>

namespace vana {
namespace packets {

PACKET_IMPL(send_password, server_type server_type, const string &pass, const ip_matrix &ext_ip) {
	packet_builder builder;
	builder.add<packet_header>(IMSG_PASSWORD);
	builder.add<string>(pass);
	builder.add<vector<external_ip>>(ext_ip);
	builder.add<server_type_underlying>(static_cast<server_type_underlying>(server_type));
	return builder;
}

}
}