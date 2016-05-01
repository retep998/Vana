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
#include "ServerAcceptedSession.hpp"
#include "Common/AbstractServer.hpp"
#include "Common/AuthenticationPacket.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include <asio.hpp>
#include <iostream>

namespace Vana {

ServerAcceptedSession::ServerAcceptedSession(AbstractServer &server) :
	m_server{server}
{
}

auto ServerAcceptedSession::handle(PacketReader &reader) -> Result {
	if (reader.get<header_t>() == IMSG_PASSWORD) {
		if (reader.get<string_t>() == m_server.getInterPassword()) {
			m_isAuthenticated = true;

			setExternalIpInformation(m_session->getIp(), reader.get<vector_t<ExternalIp>>());

			ServerType type = static_cast<ServerType>(reader.get<server_type_t>());
			m_type = type;
			authenticated(type);
		}
		else {
			m_server.log(LogType::ServerAuthFailure, [&](out_stream_t &log) {
				log << "IP: " << m_session->getIp();
			});
			m_session->disconnect();
			return Result::Failure;
		}
	}
	else if (!m_isAuthenticated) {
		m_session->disconnect();
		return Result::Failure;
	}
	reader.reset();
	return Result::Successful;
}

auto ServerAcceptedSession::authenticated(ServerType type) -> void {
	// Intentionally blank
}

auto ServerAcceptedSession::getType() const -> ServerType {
	return m_type;
}

auto ServerAcceptedSession::isAuthenticated() const -> bool {
	return m_isAuthenticated;
}

auto ServerAcceptedSession::getExternalIps() const -> const IpMatrix & {
	return m_resolver.getExternalIps();
}

auto ServerAcceptedSession::matchSubnet(const Ip &test) const -> Ip {
	return m_resolver.matchIpToSubnet(test);
}

auto ServerAcceptedSession::setExternalIpInformation(const Ip &defaultIp, const IpMatrix &matrix) -> void {
	m_resolver.setExternalIpInformation(defaultIp, matrix);
}

auto ServerAcceptedSession::onConnect() -> void {
	// Intentionally blank
}

auto ServerAcceptedSession::onDisconnect() -> void {
	// Intentionally blank
}

}