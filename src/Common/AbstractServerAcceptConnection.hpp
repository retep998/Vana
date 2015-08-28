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
#pragma once

#include "AbstractConnection.hpp"
#include "ExternalIp.hpp"
#include "ExternalIpResolver.hpp"
#include "ServerType.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	class AbstractServer;
	class PacketReader;

	class AbstractServerAcceptConnection : public AbstractConnection {
	public:
		auto getType() const -> ServerType { return m_type; }
		auto matchSubnet(const Ip &test) const -> Ip { return m_resolver.matchIpToSubnet(test); }
		auto setExternalIpInformation(const Ip &defaultIp, const IpMatrix &matrix) -> void { m_resolver.setExternalIpInformation(defaultIp, matrix); }
	protected:
		AbstractServerAcceptConnection() :
			AbstractConnection{true}
		{
		}

		auto processAuth(AbstractServer &server, PacketReader &reader) -> Result;
		virtual auto authenticated(ServerType type) -> void = 0;
		auto isAuthenticated() const -> bool { return m_isAuthenticated; }
		auto getExternalIps() const -> const IpMatrix & { return m_resolver.getExternalIps(); }
	private:
		bool m_isAuthenticated = false;
		ServerType m_type = ServerType::None;
		ExternalIpResolver m_resolver;
	};
}