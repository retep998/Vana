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
#include "World.h"
#include "Worlds.h"
#include <boost/lexical_cast.hpp>

LoginServerAcceptConnection::~LoginServerAcceptConnection() {
	if (worldId != -1) {
		World *world = Worlds::Instance()->getWorld(worldId);
		world->setConnected(false);
		world->clearChannels(); // Remove the channels (they will automaticly disconnect)
		world->setCashServerConnected(false);
		LoginServer::Instance()->log(LogTypes::ServerDisconnect, "World " + boost::lexical_cast<string>(static_cast<int16_t>(worldId)));
	}
}

void LoginServerAcceptConnection::realHandleRequest(PacketReader &packet) {
	if (!processAuth(LoginServer::Instance(), packet, LoginServer::Instance()->getInterPassword())) return;
	switch (packet.getHeader()) {
		case IMSG_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, packet); break;
		case IMSG_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(this, packet); break;
		case IMSG_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(this, packet); break;
		case IMSG_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case IMSG_TO_WORLDS: LoginServerAcceptHandler::toWorlds(this, packet); break;
		case IMSG_REMOVE_CASH_SERVER: LoginServerAcceptHandler::removeCashServer(this, packet); break;
		case IMSG_REGISTER_CASH_SERVER: LoginServerAcceptHandler::registerCashServer(this, packet); break;
	}
}

void LoginServerAcceptConnection::authenticated(int8_t type) {
	switch (type) {
		case ServerTypes::World: Worlds::Instance()->addWorldServer(this); break;
		case ServerTypes::Channel: Worlds::Instance()->addChannelServer(this); break;
		case ServerTypes::Cash: Worlds::Instance()->addCashServer(this); break;
		default: getSession()->disconnect();
	}
}
