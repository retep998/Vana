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
#include "WorldServerSession.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/ServerType.hpp"
#include "Common/WorldConfig.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/PartyHandler.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/SyncHandler.hpp"
#include "ChannelServer/WorldServerSessionHandler.hpp"
#include "ChannelServer/WorldServerPacket.hpp"
#include <iostream>

namespace Vana {
namespace ChannelServer {

auto WorldServerSession::handle(PacketReader &reader) -> Result {
	switch (reader.get<header_t>()) {
		case IMSG_CHANNEL_CONNECT: WorldServerSessionHandler::connect(shared_from_this(), reader); break;
		case IMSG_TO_PLAYER: {
			player_id_t playerId = reader.get<player_id_t>();
			ChannelServer::getInstance().getPlayerDataProvider().send(playerId, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector_t<player_id_t> playerIds = reader.get<vector_t<player_id_t>>();
			ChannelServer::getInstance().getPlayerDataProvider().send(playerIds, Vana::Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: ChannelServer::getInstance().getPlayerDataProvider().send(Vana::Packets::identity(reader)); break;
		case IMSG_REFRESH_DATA: WorldServerSessionHandler::reloadMcdb(reader); break;
		case IMSG_REHASH_CONFIG: ChannelServer::getInstance().setConfig(reader.get<WorldConfig>()); break;
		case IMSG_SYNC: SyncHandler::handle(reader); break;
		default: return Result::Failure;
	}
	return Result::Successful;
}

auto WorldServerSession::onConnect() -> void {
	ChannelServer::getInstance().onConnectToWorld(shared_from_this());
}

auto WorldServerSession::onDisconnect() -> void {
	ChannelServer::getInstance().onDisconnectFromWorld();
}

}
}