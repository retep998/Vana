/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "InterHeader.h"
#include "LoginServerConnectHandler.h"
#include "PacketReader.h"
#include "SyncHandler.h"
#include "WorldServer.h"
#include "WorldServerAcceptHandler.h"
#include <iostream>

LoginServerConnection::LoginServerConnection() {
	setType(ServerTypes::World);
}

LoginServerConnection::~LoginServerConnection() {
	if (WorldServer::Instance()->isConnected()) {
		WorldServer::Instance()->setWorldId(-1);
		std::cout << "Disconnected from loginserver. Shutting down..." << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnection::realHandleRequest(PacketReader &packet) {
	switch (packet.getHeader()) {
		case IMSG_WORLD_CONNECT: LoginServerConnectHandler::connect(this, packet); break;
		case IMSG_NEW_PLAYER: LoginServerConnectHandler::newPlayer(packet); break;
		case IMSG_TO_CHANNELS: WorldServerAcceptHandler::sendToChannels(packet); break;
		case IMSG_GUILD_OPERATION: SyncHandler::handleLoginServerPacket(this, packet); break;
		case IMSG_TO_CASH_SERVER: WorldServerAcceptHandler::sendToCashServer(packet); break;
	}
}
