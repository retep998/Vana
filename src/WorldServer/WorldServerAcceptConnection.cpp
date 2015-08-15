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
#include "WorldServerAcceptConnection.hpp"
#include "Channels.hpp"
#include "InterHeader.hpp"
#include "LoginServerConnectPacket.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "SyncHandler.hpp"
#include "SyncPacket.hpp"
#include "VanaConstants.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptPacket.hpp"
#include <iostream>

WorldServerAcceptConnection::~WorldServerAcceptConnection() {
	if (isAuthenticated()) {
		if (getType() == ServerType::Channel) {
			auto &server = WorldServer::getInstance();
			if (server.isConnected()) {
				server.sendLogin(LoginServerConnectPacket::removeChannel(m_channel));
			}
			server.getPlayerDataProvider().channelDisconnect(m_channel);
			server.getChannels().removeChannel(m_channel);

			server.log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
	}
}

auto WorldServerAcceptConnection::handleRequest(PacketReader &reader) -> void {
	auto &server = WorldServer::getInstance();
	if (processAuth(server, reader) == Result::Failure) {
		return;
	}
	switch (reader.get<header_t>()) {
		case IMSG_SYNC: SyncHandler::handle(this, reader); break;
		case IMSG_TO_LOGIN: server.sendLogin(Packets::identity(reader)); break;
		case IMSG_TO_PLAYER: {
			player_id_t playerId = reader.get<player_id_t>();
			server.getPlayerDataProvider().send(playerId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<player_id_t> playerIds = reader.get<vector_t<player_id_t>>();
			server.getPlayerDataProvider().send(playerIds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: server.getPlayerDataProvider().send(Packets::identity(reader)); break;
		case IMSG_TO_CHANNEL: {
			channel_id_t channelId = reader.get<channel_id_t>();
			server.getChannels().send(channelId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector_t<channel_id_t> channels = reader.get<vector_t<channel_id_t>>();
			server.getChannels().send(channels, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: server.getChannels().send(Packets::identity(reader)); break;
	}
}

auto WorldServerAcceptConnection::authenticated(ServerType type) -> void {
	if (type == ServerType::Channel) {
		auto &server = WorldServer::getInstance();
		m_channel = server.getChannels().getFirstAvailableChannelId();
		if (m_channel != -1) {
			port_t port = server.makeChannelPort(m_channel);
			const IpMatrix &ips = getExternalIps();
			server.getChannels().registerChannel(this, m_channel, getIp(), ips, port);

			send(WorldServerAcceptPacket::connect(m_channel, port));

			// TODO FIXME packet - a more elegant way to do this?
			send(SyncPacket::sendSyncData([&](PacketBuilder &builder) {
				server.getPlayerDataProvider().getChannelConnectPacket(builder);
			}));

			server.sendLogin(LoginServerConnectPacket::registerChannel(m_channel, getIp(), ips, port));

			server.log(LogType::ServerConnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
		else {
			send(WorldServerAcceptPacket::connect(-1, 0));
			server.log(LogType::Error, "ERROR: No more channels to assign.");
			disconnect();
		}
	}
}

auto WorldServerAcceptConnection::getChannel() const -> channel_id_t {
	return m_channel;
}