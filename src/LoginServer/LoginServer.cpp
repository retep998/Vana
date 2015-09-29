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
#include "LoginServer.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/InitializeCommon.hpp"
#include "Common/MajorBossConfig.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/RatesConfig.hpp"
#include "Common/SaltingConfig.hpp"
#include "Common/ServerType.hpp"
#include "LoginServer/LoginServerAcceptPacket.hpp"
#include "LoginServer/RankingCalculator.hpp"
#include "LoginServer/UserConnection.hpp"
#include "LoginServer/World.hpp"
#include "LoginServer/Worlds.hpp"
#include <sstream>
#include <iostream>

namespace Vana {
namespace LoginServer {

LoginServer::LoginServer() :
	AbstractServer{ServerType::Login}
{
}

auto LoginServer::listen() -> void {
	auto &config = getInterServerConfig();
	getConnectionManager().accept(Ip::Type::Ipv4, m_port, [] { return new UserConnection{}; }, config, false, MapleVersion::LoginSubversion);
	getConnectionManager().accept(Ip::Type::Ipv4, config.loginPort, [] { return new LoginServerAcceptConnection{}; }, config, true, MapleVersion::LoginSubversion);
}

auto LoginServer::loadData() -> Result {
	if (Initializing::checkSchemaVersion(this, true) == Result::Failure) {
		return Result::Failure;
	}
	if (Initializing::checkMcdbVersion(this) == Result::Failure) {
		return Result::Failure;
	}
	Initializing::setUsersOffline(this, 1);

	m_validCharDataProvider.loadData();
	m_equipDataProvider.loadData();
	m_curseDataProvider.loadData();

	RankingCalculator::setTimer();
	displayLaunchTime();

	return Result::Successful;
}

auto LoginServer::loadConfig() -> Result {
	auto config = ConfigFile::getLoginServerConfig();
	config->run();
	m_pinEnabled = config->get<bool>("pin");
	m_port = config->get<port_t>("port");
	m_maxInvalidLogins = config->get<int32_t>("invalid_login_threshold");

	auto salting = ConfigFile::getSaltingConfig();
	salting->run();

	auto saltingConf = salting->get<SaltingConfig>("");
	m_accountSaltingPolicy = saltingConf.account;
	m_accountSaltSize = saltingConf.accountSaltSize;

	loadWorlds();

	return Result::Successful;
}

auto LoginServer::initComplete() -> void {
	listen();
}

auto LoginServer::makeLogIdentifier() const -> opt_string_t {
	// Login needs no special identifier; there's only one
	return opt_string_t{};
}

auto LoginServer::getLogPrefix() const -> string_t {
	return "login";
}

auto LoginServer::getPinEnabled() const -> bool {
	return m_pinEnabled;
}

auto LoginServer::rehashConfig() -> void {
	loadWorlds();
	m_worlds.runFunction([](World *world) -> bool {
		if (world != nullptr && world->isConnected()) {
			// We only need to inform worlds that are actually connected
			// Otherwise they'll get the modified config when they connect
			world->send(Packets::Interserver::rehashConfig(world));
		}
		return false;
	});
}

auto LoginServer::getInvalidLoginThreshold() const -> int32_t {
	return m_maxInvalidLogins;
}

auto LoginServer::getValidCharDataProvider() const -> const ValidCharDataProvider & {
	return m_validCharDataProvider;
}

auto LoginServer::getEquipDataProvider() const -> const EquipDataProvider & {
	return m_equipDataProvider;
}

auto LoginServer::getCurseDataProvider() const -> const CurseDataProvider & {
	return m_curseDataProvider;
}

auto LoginServer::getWorlds() -> Worlds & {
	return m_worlds;
}

auto LoginServer::getCharacterAccountSaltSize() const -> const SaltSizeConfig & {
	return m_accountSaltSize;
}

auto LoginServer::getCharacterAccountSaltingPolicy() const -> const SaltConfig & {
	return m_accountSaltingPolicy;
}

auto LoginServer::loadWorlds() -> void {
	auto config = ConfigFile::getWorldsConfig();
	config->run();

	LuaVariant worldsConfig = config->get<LuaVariant>("worlds");
	if (!worldsConfig.is(LuaType::Table)) {
		config->error("worlds must be a table");
	}

	auto &map = worldsConfig.as<ord_map_t<int32_t, LuaVariant>>();
	for (const auto &world : map) {
		auto worldConfig = world.second.into<WorldConfig>(*config, "worlds." + std::to_string(world.first));

		World *world = m_worlds.getWorld(worldConfig.id);
		bool added = (world == nullptr);
		if (added) {
			world = new World{};
		}

		world->setConfiguration(worldConfig);
		if (added) {
			world->setId(worldConfig.id);
			world->setPort(worldConfig.basePort);
			m_worlds.addWorld(world);
		}
	}
}

}
}