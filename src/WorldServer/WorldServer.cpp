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
#include "WorldServer.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/InitializeCommon.hpp"
#include "Common/ServerType.hpp"
#include "Common/StringUtilities.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/SyncPacket.hpp"
#include "WorldServer/WorldServerAcceptPacket.hpp"

namespace Vana {
namespace WorldServer {

WorldServer::WorldServer() :
	AbstractServer{ServerType::World}
{
}

auto WorldServer::shutdown() -> void {
	// If we don't do this and the connection disconnects, it will try to call shutdown() again
	m_worldId = -1;
	AbstractServer::shutdown();
}

auto WorldServer::listen() -> void {
	getConnectionManager().accept(Ip::Type::Ipv4, m_port, [] { return new WorldServerAcceptConnection{}; }, getInterServerConfig(), true, MapleVersion::LoginSubversion);
}

auto WorldServer::loadData() -> Result {
	Initializing::checkSchemaVersion(this);

	m_loginConnection = new LoginServerConnection{};
	auto &config = getInterServerConfig();

	if (getConnectionManager().connect(config.loginIp, config.loginPort, config, m_loginConnection) == Result::Failure) {
		return Result::Failure;
	}

	sendAuth(m_loginConnection);
	return Result::Successful;
}

auto WorldServer::rehashConfig(const WorldConfig &config) -> void {
	m_config = config;
	m_defaultRates = config.rates;
	m_channels.send(Packets::Interserver::rehashConfig(config));
}

auto WorldServer::establishedLoginConnection(world_id_t worldId, port_t port, const WorldConfig &conf) -> void {
	m_worldId = worldId;

	log(LogType::ServerConnect, [&](out_stream_t &str) {
		str << "Handling world " << static_cast<int32_t>(worldId);
	});

	m_port = port;
	m_config = conf;
	m_defaultRates = conf.rates;
	listen();

	m_playerDataProvider.loadData();

	displayLaunchTime();
}

auto WorldServer::setRates(const RatesConfig &rates) -> void {
	m_config.rates = rates;
	m_channels.send(Packets::Interserver::Config::setRates(rates));
}

auto WorldServer::resetRates(int32_t flags) -> void {
	if ((flags & RatesConfig::Types::all) == RatesConfig::Types::all) {
		setRates(m_defaultRates);
	}
	else {
		RatesConfig newRates = m_config.rates;
		if ((flags & RatesConfig::Types::mobExpRate) == RatesConfig::Types::mobExpRate) {
			newRates.mobExpRate = m_defaultRates.mobExpRate;
		}
		if ((flags & RatesConfig::Types::questExpRate) == RatesConfig::Types::questExpRate) {
			newRates.questExpRate = m_defaultRates.questExpRate;
		}
		if ((flags & RatesConfig::Types::dropRate) == RatesConfig::Types::dropRate) {
			newRates.dropRate = m_defaultRates.dropRate;
		}
		if ((flags & RatesConfig::Types::dropMeso) == RatesConfig::Types::dropMeso) {
			newRates.dropMeso = m_defaultRates.dropMeso;
		}
		if ((flags & RatesConfig::Types::globalDropRate) == RatesConfig::Types::globalDropRate) {
			newRates.globalDropRate = m_defaultRates.globalDropRate;
		}
		if ((flags & RatesConfig::Types::globalDropMeso) == RatesConfig::Types::globalDropMeso) {
			newRates.globalDropMeso = m_defaultRates.globalDropMeso;
		}
		setRates(newRates);
	}
}

auto WorldServer::getPlayerDataProvider() -> PlayerDataProvider & {
	return m_playerDataProvider;
}

auto WorldServer::getChannels() -> Channels & {
	return m_channels;
}

auto WorldServer::makeLogIdentifier() const -> opt_string_t {
	return buildLogIdentifier([&](out_stream_t &id) { id << "World " << static_cast<int32_t>(m_worldId); });
}

auto WorldServer::getLogPrefix() const -> string_t {
	return "world";
}

auto WorldServer::isConnected() const -> bool {
	return m_worldId != -1;
}

auto WorldServer::getWorldId() const -> world_id_t {
	return m_worldId;
}

auto WorldServer::makeChannelPort(channel_id_t channelId) const -> port_t {
	return m_port + channelId + 1;
}

auto WorldServer::getConfig() -> const WorldConfig & {
	return m_config;
}

auto WorldServer::setScrollingHeader(const string_t &message) -> void {
	m_config.scrollingHeader = message;
	m_channels.send(Packets::Interserver::Config::scrollingHeader(message));
}

auto WorldServer::sendLogin(const PacketBuilder &builder) -> void {
	m_loginConnection->send(builder);
}

}
}