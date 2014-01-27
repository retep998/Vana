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
#include "LoginServerAcceptConnection.h"
#include "InterHeader.h"
#include "LoginServer.h"
#include "LoginServerAcceptHandler.h"
#include "PacketReader.h"
#include "RankingCalculator.h"
#include "Session.h"
#include "StringUtilities.h"
#include "VanaConstants.h"
#include "World.h"
#include "Worlds.h"

LoginServerAcceptConnection::~LoginServerAcceptConnection() {
	if (m_worldId != -1) {
		World *world = Worlds::getInstance().getWorld(m_worldId);
		world->setConnected(false);
		world->clearChannels();

		LoginServer::getInstance().log(LogTypes::ServerDisconnect, "World " + StringUtilities::lexical_cast<string_t>(m_worldId));
	}
}

auto LoginServerAcceptConnection::handleRequest(PacketReader &packet) -> void {
	if (!processAuth(LoginServer::getInstance(), packet)) return;
	switch (packet.getHeader()) {
		case IMSG_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, packet); break;
		case IMSG_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(this, packet); break;
		case IMSG_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(this, packet); break;
		case IMSG_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case IMSG_TO_WORLDS: LoginServerAcceptHandler::sendPacketToWorlds(this, packet); break;
		case IMSG_REHASH_CONFIG: LoginServer::getInstance().rehashConfig(); break;
	}
}

auto LoginServerAcceptConnection::authenticated(int8_t type) -> void {
	switch (type) {
		case ServerTypes::World: Worlds::getInstance().addWorldServer(this); break;
		case ServerTypes::Channel: Worlds::getInstance().addChannelServer(this); break;
		default: getSession()->disconnect();
	}
}