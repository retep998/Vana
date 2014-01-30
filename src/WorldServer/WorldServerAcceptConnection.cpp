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
#include "WorldServerAcceptConnection.hpp"
#include "Channels.hpp"
#include "InterHeader.hpp"
#include "LoginServerConnectPacket.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "SyncHandler.hpp"
#include "SyncPacket.hpp"
#include "VanaConstants.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptHandler.hpp"
#include "WorldServerAcceptPacket.hpp"
#include <iostream>

WorldServerAcceptConnection::~WorldServerAcceptConnection() {
	if (isAuthenticated()) {
		if (getType() == ServerType::Channel) {
			if (WorldServer::getInstance().isConnected()) {
				LoginServerConnectPacket::removeChannel(m_channel);
			}
			PlayerDataProvider::getInstance().channelDisconnect(m_channel);
			Channels::getInstance().removeChannel(m_channel);

			WorldServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
	}
}

auto WorldServerAcceptConnection::handleRequest(PacketReader &packet) -> void {
	if (processAuth(WorldServer::getInstance(), packet) == Result::Failure) {
		return;
	}
	switch (packet.getHeader()) {
		case IMSG_SYNC: SyncHandler::handle(this, packet); break;
		case IMSG_TO_LOGIN: WorldServerAcceptHandler::sendPacketToLogin(packet); break;
		case IMSG_TO_PLAYER: PlayerDataProvider::getInstance().forwardPacketToPlayer(packet); break;
		case IMSG_TO_PLAYER_LIST: PlayerDataProvider::getInstance().forwardPacketToPlayerList(packet); break;
		case IMSG_TO_ALL_PLAYERS: PlayerDataProvider::getInstance().forwardPacketToAllPlayers(packet); break;
		case IMSG_TO_CHANNEL: WorldServerAcceptHandler::sendPacketToChannel(packet); break;
		case IMSG_TO_CHANNEL_LIST: WorldServerAcceptHandler::sendPacketToChannelList(packet); break;
		case IMSG_TO_ALL_CHANNELS: WorldServerAcceptHandler::sendPacketToAllChannels(packet); break;
	}
}

auto WorldServerAcceptConnection::authenticated(ServerType type) -> void {
	if (type == ServerType::Channel) {
		m_channel = Channels::getInstance().getAvailableChannel();
		if (m_channel != -1) {
			port_t port = WorldServer::getInstance().makeChannelPort(m_channel);
			const IpMatrix &ips = getExternalIps();
			Channels::getInstance().registerChannel(this, m_channel, getIp(), ips, port);

			WorldServerAcceptPacket::connect(this, m_channel, port);
			SyncPacket::sendSyncData(this);
			LoginServerConnectPacket::registerChannel(m_channel, getIp(), ips, port);

			WorldServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
		else {
			WorldServerAcceptPacket::connect(this, -1, 0);
			std::cerr << "ERROR: No more channels to assign." << std::endl;
			getSession()->disconnect();
		}
	}
}