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
#include "ChannelServer.hpp"
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include "ConnectionManager.hpp"
#include "InitializeChannel.hpp"
#include "InitializeCommon.hpp"
#include "MiscUtilities.hpp"
#include "PacketBuilder.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "ServerPacket.hpp"
#include "SyncPacket.hpp"
#include "VanaConstants.hpp"
#include "WorldServerConnection.hpp"
#include "WorldServerConnectPacket.hpp"

ChannelServer::ChannelServer() :
	AbstractServer(ServerType::Channel),
	m_worldIp(0)
{
}

auto ChannelServer::listen() -> void {
	ConnectionManager::getInstance().accept(Ip::Type::Ipv4, m_port, [] { return new Player(); }, getInterServerConfig(), false);
	Initializing::setUsersOffline(getOnlineId());
}

auto ChannelServer::shutdown() -> void {
	// If we don't do this and the connection disconnects, it will try to call shutdown() again
	m_channelId = -1;
	AbstractServer::shutdown();
}

auto ChannelServer::loadData() -> void {
	Initializing::checkSchemaVersion();
	Initializing::checkMcdbVersion();
	Initializing::loadData();

	WorldServerConnection *loginPlayer = new WorldServerConnection;
	auto &config = getInterServerConfig();
	ConnectionManager::getInstance().connect(config.loginIp, config.loginPort, config, loginPlayer);
	sendAuth(loginPlayer);
}

auto ChannelServer::makeLogIdentifier() const -> opt_string_t {
	return buildLogIdentifier([&](out_stream_t &id) { id << "World: " << static_cast<int32_t>(m_worldId) << "; ID: " << static_cast<int32_t>(m_channelId); });
}

auto ChannelServer::getLogPrefix() const -> string_t {
	return "channel";
}

auto ChannelServer::connectToWorld(world_id_t worldId, port_t port, const Ip &ip) -> void {
	m_worldId = worldId;
	m_worldPort = port;
	m_worldIp = ip;

	m_worldConnection = new WorldServerConnection;
	ConnectionManager::getInstance().connect(ip, port, getInterServerConfig(), m_worldConnection);
	sendAuth(m_worldConnection);
}

auto ChannelServer::establishedWorldConnection(channel_id_t channelId, port_t port, const WorldConfig &config) -> void {
	m_channelId = channelId;
	m_port = port;
	m_config = config;
	listen();
	displayLaunchTime();
}

auto ChannelServer::getConfig() const -> const WorldConfig & {
	return m_config;
}

auto ChannelServer::isConnected() const -> bool {
	return m_channelId != -1;
}

auto ChannelServer::getWorldId() const -> world_id_t {
	return m_worldId;
}

auto ChannelServer::getChannelId() const -> channel_id_t {
	return m_channelId;
}

auto ChannelServer::getOnlineId() const -> int32_t {
	return 20000 + static_cast<int32_t>(m_worldId) * 100 + m_channelId;
}

auto ChannelServer::sendWorld(const PacketBuilder &builder) -> void {
	m_worldConnection->send(builder);
}

auto ChannelServer::setScrollingHeader(const string_t &message) -> void {
	if (m_config.scrollingHeader != message) {
		m_config.scrollingHeader = message;
		PlayerDataProvider::getInstance().send(ServerPacket::changeScrollingHeader(message));
	}
}

auto ChannelServer::modifyRate(int32_t rateType, int32_t newValue) -> void {
	Rates currentRates = m_config.rates;
	if (rateType & Rates::Types::MobExpRate) currentRates.mobExpRate = newValue;
	if (rateType & Rates::Types::MobMesoRate) currentRates.mobMesoRate = newValue;
	if (rateType & Rates::Types::QuestExpRate) currentRates.questExpRate = newValue;
	if (rateType & Rates::Types::DropRate) currentRates.dropRate = newValue;
	sendWorld(SyncPacket::ConfigPacket::modifyRates(currentRates));
}

auto ChannelServer::setRates(const Rates &rates) -> void {
	m_config.rates = rates;
}

auto ChannelServer::setConfig(const WorldConfig &config) -> void {
	setScrollingHeader(config.scrollingHeader);
	m_config = config;
}