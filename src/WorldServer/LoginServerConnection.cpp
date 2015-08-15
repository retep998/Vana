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
#include "LoginServerConnection.hpp"
#include "Channels.hpp"
#include "ExitCodes.hpp"
#include "LoginServerConnectHandler.hpp"
#include "InterHeader.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "SyncHandler.hpp"
#include "VanaConstants.hpp"
#include "WorldServer.hpp"
#include <iostream>

LoginServerConnection::LoginServerConnection() :
	AbstractServerConnection{ServerType::World}
{
}

LoginServerConnection::~LoginServerConnection() {
	if (WorldServer::getInstance().isConnected()) {
		WorldServer::getInstance().log(LogType::ServerDisconnect, "Disconnected from the LoginServer. Shutting down...");
		ExitCodes::exit(ExitCodes::ServerDisconnection);
	}
}

auto LoginServerConnection::handleRequest(PacketReader &reader) -> void {
	switch (reader.get<header_t>()) {
		case IMSG_WORLD_CONNECT: LoginServerConnectHandler::connect(this, reader); break;
		case IMSG_REHASH_CONFIG: WorldServer::getInstance().rehashConfig(reader.get<WorldConfig>()); break;
		case IMSG_TO_CHANNEL: {
			channel_id_t channelId = reader.get<channel_id_t>();
			WorldServer::getInstance().getChannels().send(channelId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector_t<channel_id_t> channels = reader.get<vector_t<channel_id_t>>();
			WorldServer::getInstance().getChannels().send(channels, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: WorldServer::getInstance().getChannels().send(Packets::identity(reader)); break;
		case IMSG_SYNC: SyncHandler::handle(this, reader); break;
	}
}