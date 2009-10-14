/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "LoginServerConnection.h"
#include "GuildHandler.h"
#include "LoginServerConnectHandler.h"
#include "InterHeader.h"
#include "PacketReader.h"
#include "WorldServer.h"
#include "WorldServerAcceptHandler.h"
#include <iostream>

LoginServerConnection::LoginServerConnection() {
	setType(InterWorldServer);
}

LoginServerConnection::~LoginServerConnection() {
	if (WorldServer::Instance()->isConnected()) {
		WorldServer::Instance()->setWorldId(-1);
		std::cout << "Disconnected from loginserver. Shutting down..." << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnection::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case INTER_WORLD_CONNECT: LoginServerConnectHandler::connect(this, packet); break;
		case INTER_NEW_PLAYER: LoginServerConnectHandler::newPlayer(packet); break;
		case INTER_TO_CHANNELS: WorldServerAcceptHandler::toChannels(packet); break;
		case INTER_GUILD_OPERATION: GuildHandler::handleLoginServerPacket(this, packet); break;
	}
}
