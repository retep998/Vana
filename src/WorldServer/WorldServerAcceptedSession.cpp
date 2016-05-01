/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "WorldServerAcceptedSession.hpp"
#include "Common/InterHeader.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/ServerType.hpp"
#include "Common/Session.hpp"
#include "Common/StringUtilities.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/LoginServerConnectPacket.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/SyncHandler.hpp"
#include "WorldServer/SyncPacket.hpp"
#include "WorldServer/WorldServer.hpp"
#include "WorldServer/WorldServerAcceptPacket.hpp"
#include <iostream>

namespace Vana {
namespace WorldServer {

WorldServerAcceptedSession::WorldServerAcceptedSession(AbstractServer &server) :
	ServerAcceptedSession{server}
{
}

auto WorldServerAcceptedSession::handle(PacketReader &reader) -> Result {
	if (ServerAcceptedSession::handle(reader) == Result::Failure) {
		return Result::Failure;
	}

	auto &server = WorldServer::getInstance();
	switch (reader.get<header_t>()) {
		case IMSG_SYNC: SyncHandler::handle(shared_from_this(), reader); break;
		case IMSG_TO_LOGIN: server.sendLogin(Vana::Packets::identity(reader)); break;
		case IMSG_TO_PLAYER: {
			player_id_t playerId = reader.get<player_id_t>();
			server.getPlayerDataProvider().send(playerId, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<player_id_t> playerIds = reader.get<vector_t<player_id_t>>();
			server.getPlayerDataProvider().send(playerIds, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: server.getPlayerDataProvider().send(Vana::Packets::identity(reader)); break;
		case IMSG_TO_CHANNEL: {
			channel_id_t channelId = reader.get<channel_id_t>();
			server.getChannels().send(channelId, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector_t<channel_id_t> channels = reader.get<vector_t<channel_id_t>>();
			server.getChannels().send(channels, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: server.getChannels().send(Vana::Packets::identity(reader)); break;
	}
	return Result::Successful;
}

auto WorldServerAcceptedSession::authenticated(ServerType type) -> void {
	if (type == ServerType::Channel) {
		auto &server = WorldServer::getInstance();
		m_channel = server.getChannels().getFirstAvailableChannelId();
		if (m_channel != -1) {
			auto ip = getIp().get(Ip{0});
			port_t port = server.makeChannelPort(m_channel);
			const IpMatrix &ips = getExternalIps();
			server.getChannels().registerChannel(shared_from_this(), m_channel, ip, ips, port);

			send(Packets::Interserver::connect(m_channel, port));

			// TODO FIXME packet - a more elegant way to do this?
			send(Packets::Interserver::sendSyncData([&](PacketBuilder &builder) {
				server.getPlayerDataProvider().getChannelConnectPacket(builder);
			}));

			server.sendLogin(Packets::registerChannel(m_channel, ip, ips, port));

			server.log(LogType::ServerConnect, [&](out_stream_t &log) {
				log << "Channel " << static_cast<int32_t>(m_channel);
			});
		}
		else {
			send(Packets::Interserver::connect(-1, 0));
			server.log(LogType::Error, "No more channels to assign.");
			disconnect();
		}
	}
}

auto WorldServerAcceptedSession::getChannel() const -> channel_id_t {
	return m_channel;
}

auto WorldServerAcceptedSession::onDisconnect() -> void {
	if (isAuthenticated()) {
		if (getType() == ServerType::Channel) {
			auto &server = WorldServer::getInstance();
			if (server.isConnected()) {
				server.sendLogin(Packets::removeChannel(m_channel));
			}
			server.getPlayerDataProvider().channelDisconnect(m_channel);
			server.getChannels().removeChannel(m_channel);

			server.log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
	}
}

}
}