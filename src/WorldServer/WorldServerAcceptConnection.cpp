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
			if (WorldServer::getInstance().isConnected()) {
				WorldServer::getInstance().sendLogin(LoginServerConnectPacket::removeChannel(m_channel));
			}
			PlayerDataProvider::getInstance().channelDisconnect(m_channel);
			Channels::getInstance().removeChannel(m_channel);

			WorldServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
	}
}

auto WorldServerAcceptConnection::handleRequest(PacketReader &reader) -> void {
	if (processAuth(WorldServer::getInstance(), reader) == Result::Failure) {
		return;
	}
	switch (reader.get<header_t>()) {
		case IMSG_SYNC: SyncHandler::handle(this, reader); break;
		case IMSG_TO_LOGIN: WorldServer::getInstance().sendLogin(Packets::identity(reader)); break;
		case IMSG_TO_PLAYER: {
			player_id_t playerId = reader.get<player_id_t>();
			PlayerDataProvider::getInstance().send(playerId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<player_id_t> playerIds = reader.get<vector_t<player_id_t>>();
			PlayerDataProvider::getInstance().send(playerIds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: PlayerDataProvider::getInstance().send(Packets::identity(reader)); break;
		case IMSG_TO_CHANNEL: {
			channel_id_t channelId = reader.get<channel_id_t>();
			Channels::getInstance().send(channelId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector_t<channel_id_t> channels = reader.get<vector_t<channel_id_t>>();
			Channels::getInstance().send(channels, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: Channels::getInstance().send(Packets::identity(reader)); break;
	}
}

auto WorldServerAcceptConnection::authenticated(ServerType type) -> void {
	if (type == ServerType::Channel) {
		m_channel = Channels::getInstance().getFirstAvailableChannelId();
		if (m_channel != -1) {
			port_t port = WorldServer::getInstance().makeChannelPort(m_channel);
			const IpMatrix &ips = getExternalIps();
			Channels::getInstance().registerChannel(this, m_channel, getIp(), ips, port);

			send(WorldServerAcceptPacket::connect(m_channel, port));

			// TODO FIXME packet - a more elegant way to do this?
			send(SyncPacket::sendSyncData([](PacketBuilder &builder) {
				PlayerDataProvider::getInstance().getChannelConnectPacket(builder);
			}));
			
			WorldServer::getInstance().sendLogin(LoginServerConnectPacket::registerChannel(m_channel, getIp(), ips, port));

			WorldServer::getInstance().log(LogType::ServerConnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
		else {
			send(WorldServerAcceptPacket::connect(-1, 0));
			std::cerr << "ERROR: No more channels to assign." << std::endl;
			disconnect();
		}
	}
}

auto WorldServerAcceptConnection::getChannel() const -> channel_id_t {
	return m_channel;
}