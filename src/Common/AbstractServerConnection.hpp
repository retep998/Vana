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
#include "Types.hpp"
#include "VanaConstants.hpp"
#include <string>
#include <vector>

class AbstractServer;
class PacketReader;

class AbstractServerConnection : public AbstractConnection {
public:
	auto getType() const -> ServerType { return m_type; }
protected:
	AbstractServerConnection(ServerType type) :
		AbstractConnection(true),
		m_type(type)
	{
	}
private:
	friend class AbstractServer;
	auto sendAuth(const string_t &pass, const IpMatrix &extIp) -> void;
	ServerType m_type = ServerType::None;
};