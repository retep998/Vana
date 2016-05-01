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

#include "Common/ConnectionType.hpp"
#include "Common/ExternalIp.hpp"
#include "Common/ExternalIpResolver.hpp"
#include "Common/PacketHandler.hpp"
#include "Common/ServerType.hpp"
#include "Common/Session.hpp"
#include "Common/Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	class AbstractServer;
	class PacketReader;
	class ServerAcceptedSession;

	class ServerAcceptedSession : public PacketHandler {
	public:
		ServerAcceptedSession(AbstractServer &server);

		auto getType() const -> ServerType;
		auto isAuthenticated() const -> bool;
		auto getExternalIps() const -> const IpMatrix &;
		auto matchSubnet(const Ip &test) const -> Ip;
		auto setExternalIpInformation(const Ip &defaultIp, const IpMatrix &matrix) -> void;
	protected:
		virtual auto handle(PacketReader &reader) -> Result override;
		virtual auto authenticated(ServerType type) -> void;
		virtual auto onDisconnect() -> void override;
		virtual auto onConnect() -> void override;
	private:
		bool m_isAuthenticated = false;
		ServerType m_type = ServerType::None;
		AbstractServer &m_server;
		ExternalIpResolver m_resolver;
	};
}