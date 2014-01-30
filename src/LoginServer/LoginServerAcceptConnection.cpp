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
#include "LoginServerAcceptConnection.hpp"
#include "InterHeader.hpp"
#include "LoginServer.hpp"
#include "LoginServerAcceptHandler.hpp"
#include "PacketReader.hpp"
#include "RankingCalculator.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "VanaConstants.hpp"
#include "World.hpp"
#include "Worlds.hpp"

LoginServerAcceptConnection::~LoginServerAcceptConnection() {
	if (m_worldId != -1) {
		World *world = Worlds::getInstance().getWorld(m_worldId);
		world->setConnected(false);
		world->clearChannels();

		LoginServer::getInstance().log(LogType::ServerDisconnect, [&](out_stream_t &log) { log << "World " << static_cast<int32_t>(m_worldId); });
	}
}

auto LoginServerAcceptConnection::handleRequest(PacketReader &packet) -> void {
	if (processAuth(LoginServer::getInstance(), packet) == Result::Failure) {
		return;
	}
	switch (packet.getHeader()) {
		case IMSG_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, packet); break;
		case IMSG_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(this, packet); break;
		case IMSG_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(this, packet); break;
		case IMSG_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case IMSG_TO_ALL_WORLDS: LoginServerAcceptHandler::sendPacketToAllWorlds(this, packet); break;
		case IMSG_TO_WORLD_LIST: LoginServerAcceptHandler::sendPacketToWorldList(this, packet); break;
		case IMSG_TO_WORLD: LoginServerAcceptHandler::sendPacketToWorld(this, packet); break;
		case IMSG_REHASH_CONFIG: LoginServer::getInstance().rehashConfig(); break;
	}
}

auto LoginServerAcceptConnection::authenticated(ServerType type) -> void {
	switch (type) {
		case ServerType::World: Worlds::getInstance().addWorldServer(this); break;
		case ServerType::Channel: Worlds::getInstance().addChannelServer(this); break;
		default: getSession()->disconnect();
	}
}