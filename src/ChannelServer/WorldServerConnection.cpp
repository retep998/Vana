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
#include "WorldServerConnection.hpp"
#include "ChannelServer.hpp"
#include "ExitCodes.hpp"
#include "InterHeader.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "PartyHandler.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncHandler.hpp"
#include "VanaConstants.hpp"
#include "WorldServerConnectHandler.hpp"
#include "WorldServerConnectPacket.hpp"
#include <iostream>

WorldServerConnection::WorldServerConnection() :
	AbstractServerConnection{ServerType::Channel}
{
}

WorldServerConnection::~WorldServerConnection() {
	if (ChannelServer::getInstance().isConnected()) {
		ChannelServer::getInstance().log(LogType::ServerDisconnect, "Disconnected from the WorldServer. Shutting down...");
		ExitCodes::exit(ExitCodes::ServerDisconnection);
	}
}

auto WorldServerConnection::handleRequest(PacketReader &reader) -> void {
	switch (reader.get<header_t>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: WorldServerConnectHandler::connectLogin(this, reader); break;
		case IMSG_CHANNEL_CONNECT: WorldServerConnectHandler::connect(this, reader); break;
		case IMSG_TO_PLAYER: {
			player_id_t playerId = reader.get<player_id_t>();
			ChannelServer::getInstance().getPlayerDataProvider().send(playerId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<player_id_t> playerIds = reader.get<vector_t<player_id_t>>();
			ChannelServer::getInstance().getPlayerDataProvider().send(playerIds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: ChannelServer::getInstance().getPlayerDataProvider().send(Packets::identity(reader)); break;
		case IMSG_REFRESH_DATA: WorldServerConnectHandler::reloadMcdb(reader); break;
		case IMSG_REHASH_CONFIG: ChannelServer::getInstance().setConfig(reader.get<WorldConfig>()); break;
		case IMSG_SYNC: SyncHandler::handle(reader); break;
	}
}