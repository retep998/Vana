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
#include "PacketWrapper.hpp"
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

auto LoginServerAcceptConnection::handleRequest(PacketReader &reader) -> void {
	if (processAuth(LoginServer::getInstance(), reader) == Result::Failure) {
		return;
	}
	switch (reader.get<header_t>()) {
		case IMSG_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, reader); break;
		case IMSG_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(this, reader); break;
		case IMSG_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(this, reader); break;
		case IMSG_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case IMSG_TO_WORLD: {
			world_id_t worldId = reader.get<world_id_t>();
			Worlds::getInstance().send(worldId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_WORLD_LIST: {
			vector_t<world_id_t> worlds = reader.get<vector_t<world_id_t>>();
			Worlds::getInstance().send(worlds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_WORLDS: Worlds::getInstance().send(Packets::identity(reader)); break;
		case IMSG_REHASH_CONFIG: LoginServer::getInstance().rehashConfig(); break;
	}
}

auto LoginServerAcceptConnection::authenticated(ServerType type) -> void {
	switch (type) {
		case ServerType::World: Worlds::getInstance().addWorldServer(this); break;
		case ServerType::Channel: Worlds::getInstance().addChannelServer(this); break;
		default: disconnect();
	}
}

auto LoginServerAcceptConnection::setWorldId(world_id_t id) -> void {
	m_worldId = id;
}

auto LoginServerAcceptConnection::getWorldId() const -> world_id_t {
	return m_worldId;
}
