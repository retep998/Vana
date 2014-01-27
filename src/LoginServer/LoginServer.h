/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "AbstractServer.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "InitializeLogin.h"
#include "LoginServerAcceptConnection.h"
#include "Player.h"
#include "Types.h"

class LoginServer : public AbstractServer {
	SINGLETON_CUSTOM_CONSTRUCTOR(LoginServer);
public:
	auto loadData() -> void override;
	auto loadConfig() -> void override;
	auto loadLogConfig() -> void override;
	auto loadWorlds() -> void;
	auto listen() -> void override;
	auto makeLogIdentifier() -> opt_string_t override;

	auto getPinEnabled() const -> bool { return m_pinEnabled; }
	auto setPinEnabled(bool enabled) -> void { m_pinEnabled = enabled; }
	auto rehashConfig() -> void;
	auto getInvalidLoginThreshold() const -> int32_t { return m_maxInvalidLogins; }
private:
	bool m_pinEnabled = false;
	port_t m_port = 0;
	port_t m_interPort = 0;
	int32_t m_maxInvalidLogins = 0;
};