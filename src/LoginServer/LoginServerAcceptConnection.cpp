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
#include "LoginServerAcceptConnection.h"
#include "InterHeader.h"
#include "LoginServer.h"
#include "LoginServerAcceptHandler.h"
#include "MapleSession.h"
#include "PacketReader.h"
#include "RankingCalculator.h"
#include "Worlds.h"
#include <iostream>

LoginServerAcceptConnection::~LoginServerAcceptConnection() {
	if (worldId != -1) {
		Worlds::worlds[worldId]->connected = false;
		Worlds::worlds[worldId]->channels.clear(); // Remove the channels (they will automaticly disconnect)
		std::cout << "World " << (int32_t) worldId << " disconnected." << std::endl;
	}
}

void LoginServerAcceptConnection::realHandleRequest(PacketReader &packet) {
	if (!processAuth(packet, LoginServer::Instance()->getInterPassword())) return;
	switch (packet.get<int16_t>()) {
		case INTER_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, packet); break;
		case INTER_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(this, packet); break;
		case INTER_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(this, packet); break;
		case INTER_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case INTER_TO_WORLDS: LoginServerAcceptHandler::toWorlds(this, packet); break;
	}
}

void LoginServerAcceptConnection::authenticated(int8_t type) {
	switch (type) {
		case InterWorldServer:
			Worlds::connectWorldServer(this);
			break;
		case InterChannelServer:
			Worlds::connectChannelServer(this);
			break;
		default:
			getSession()->disconnect();
	}
}
