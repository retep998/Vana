/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "AbstractServerAcceptConnection.hpp"
#include "AbstractServer.hpp"
#include "AuthenticationPacket.hpp"
#include "InterHeader.hpp"
#include "PacketReader.hpp"
#include "Session.hpp"
#include <asio.hpp>
#include <iostream>

namespace Vana {

auto AbstractServerAcceptConnection::processAuth(AbstractServer &server, PacketReader &reader) -> Result {
	if (reader.get<header_t>() == IMSG_PASSWORD) {
		if (reader.get<string_t>() == server.getInterPassword()) {
			m_isAuthenticated = true;

			setExternalIpInformation(getIp(), reader.get<vector_t<ExternalIp>>());

			ServerType type = static_cast<ServerType>(reader.get<server_type_t>());
			m_type = type;
			authenticated(type);
		}
		else {
			server.log(LogType::ServerAuthFailure, [&](out_stream_t &log) { log << "IP: " << getIp(); });

			disconnect();
			return Result::Failure;
		}
	}
	else if (!m_isAuthenticated) {
		disconnect();
		return Result::Failure;
	}
	reader.reset();
	return Result::Successful;
}

}