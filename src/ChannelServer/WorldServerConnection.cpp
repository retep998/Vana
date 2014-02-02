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
#include "WorldServerConnection.hpp"
#include "ChannelServer.hpp"
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
	AbstractServerConnection(ServerType::Channel)
{
}

WorldServerConnection::~WorldServerConnection() {
	if (ChannelServer::getInstance().isConnected()) {
		std::cout << "Disconnected from the WorldServer. Shutting down..." << std::endl;
		ChannelServer::getInstance().shutdown();
	}
}

auto WorldServerConnection::handleRequest(PacketReader &reader) -> void {
	switch (reader.get<header_t>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: WorldServerConnectHandler::connectLogin(this, reader); break;
		case IMSG_CHANNEL_CONNECT: WorldServerConnectHandler::connect(this, reader); break;
		case IMSG_TO_PLAYER: {
			int32_t playerId = reader.get<int32_t>();
			PlayerDataProvider::getInstance().send(playerId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<int32_t> playerIds = reader.get<vector_t<int32_t>>();
			PlayerDataProvider::getInstance().send(playerIds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: PlayerDataProvider::getInstance().send(Packets::identity(reader)); break;
		case IMSG_REFRESH_DATA: WorldServerConnectHandler::reloadMcdb(reader); break;
		case IMSG_REHASH_CONFIG: ChannelServer::getInstance().setConfig(reader.get<WorldConfig>()); break;
		case IMSG_SYNC: SyncHandler::handle(reader); break;
	}
}