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

#include "AbstractServer.hpp"
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include "CurseDataProvider.hpp"
#include "EquipDataProvider.hpp"
#include "LoginServerAcceptConnection.hpp"
#include "Types.hpp"
#include "ValidCharDataProvider.hpp"
#include "Worlds.hpp"

class LoginServer final : public AbstractServer {
	SINGLETON(LoginServer);
public:
	auto getPinEnabled() const -> bool;
	auto rehashConfig() -> void;
	auto getInvalidLoginThreshold() const -> int32_t;
	auto getValidCharDataProvider() const -> const ValidCharDataProvider &;
	auto getEquipDataProvider() const -> const EquipDataProvider &;
	auto getCurseDataProvider() const -> const CurseDataProvider &;
	auto getWorlds() -> Worlds &;
protected:
	auto initComplete() -> void override;
	auto loadData() -> Result override;
	auto loadConfig() -> Result override;
	auto listen() -> void;
	auto loadWorlds() -> void;
	auto makeLogIdentifier() const -> opt_string_t override;
	auto getLogPrefix() const -> string_t override;
private:
	bool m_pinEnabled = false;
	port_t m_port = 0;
	int32_t m_maxInvalidLogins = 0;
	ValidCharDataProvider m_validCharDataProvider;
	EquipDataProvider m_equipDataProvider;
	CurseDataProvider m_curseDataProvider;
	Worlds m_worlds;
};