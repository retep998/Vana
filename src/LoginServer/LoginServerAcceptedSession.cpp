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
#include "LoginServerAcceptedSession.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/ServerType.hpp"
#include "Common/Session.hpp"
#include "Common/StringUtilities.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/LoginServerAcceptHandler.hpp"
#include "LoginServer/RankingCalculator.hpp"
#include "LoginServer/World.hpp"
#include "LoginServer/Worlds.hpp"

namespace Vana {
namespace LoginServer {

LoginServerAcceptedSession::LoginServerAcceptedSession(AbstractServer &server) :
	ServerAcceptedSession{server}
{
}

auto LoginServerAcceptedSession::handle(PacketReader &reader) -> Result {
	if (ServerAcceptedSession::handle(reader) == Result::Failure) {
		return Result::Failure;
	}
	auto &server = LoginServer::getInstance();
	switch (reader.get<header_t>()) {
		case IMSG_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(shared_from_this(), reader); break;
		case IMSG_UPDATE_CHANNEL_POP: LoginServerAcceptHandler::updateChannelPop(shared_from_this(), reader); break;
		case IMSG_REMOVE_CHANNEL: LoginServerAcceptHandler::removeChannel(shared_from_this(), reader); break;
		case IMSG_CALCULATE_RANKING: RankingCalculator::runThread(); break;
		case IMSG_TO_WORLD: {
			world_id_t worldId = reader.get<world_id_t>();
			server.getWorlds().send(worldId, Packets::identity(reader));
			break;
		}
		case IMSG_TO_WORLD_LIST: {
			vector_t<world_id_t> worlds = reader.get<vector_t<world_id_t>>();
			server.getWorlds().send(worlds, Packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_WORLDS: server.getWorlds().send(Packets::identity(reader)); break;
		case IMSG_REHASH_CONFIG: server.rehashConfig(); break;
	}
	return Result::Successful;
}

auto LoginServerAcceptedSession::authenticated(ServerType type) -> void {
	switch (type) {
		case ServerType::World: LoginServer::getInstance().getWorlds().addWorldServer(shared_from_this()); break;
		case ServerType::Channel: LoginServer::getInstance().getWorlds().addChannelServer(shared_from_this()); break;
		default: disconnect();
	}
}

auto LoginServerAcceptedSession::setWorldId(world_id_t id) -> void {
	m_worldId = id;
}

auto LoginServerAcceptedSession::getWorldId() const -> optional_t<world_id_t> {
	return m_worldId;
}

auto LoginServerAcceptedSession::onDisconnect() -> void {
	auto &server = LoginServer::getInstance();
	if (m_worldId.is_initialized()) {

		World *world = server.getWorlds().getWorld(m_worldId.get());
		world->setConnected(false);
		world->clearChannels();

		server.log(LogType::ServerDisconnect, [&](out_stream_t &log) {
			log << "World " << static_cast<int32_t>(m_worldId.get());
		});
	}
	server.finalizeServerSession(shared_from_this());
}

}
}