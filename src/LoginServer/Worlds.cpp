/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Worlds.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include "Common/StringUtilities.hpp"
#include "LoginServer/Channel.hpp"
#include "LoginServer/Characters.hpp"
#include "LoginServer/LoginPacket.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/LoginServerAcceptedSession.hpp"
#include "LoginServer/LoginServerAcceptPacket.hpp"
#include "LoginServer/PlayerStatus.hpp"
#include "LoginServer/User.hpp"
#include "LoginServer/World.hpp"
#include <iostream>

namespace Vana {
namespace LoginServer {

auto Worlds::showWorld(ref_ptr_t<User> user) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	for (const auto &kvp : m_worlds) {
		if (kvp.second->isConnected()) {
			user->send(Packets::showWorld(kvp.second));
		}
	}
	user->send(Packets::worldEnd());
}

auto Worlds::addWorld(World *world) -> void {
	optional_t<world_id_t> worldId = world->getId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}
	m_worlds[worldId.get()] = world;
}

auto Worlds::selectWorld(ref_ptr_t<User> user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	world_id_t worldId = reader.get<world_id_t>();
	if (World *world = getWorld(worldId)) {
		int32_t load = world->getPlayerLoad();
		int32_t maxLoad = world->getMaxPlayerLoad();
		int32_t minMaxLoad = (maxLoad / 100) * 90;
		int8_t message = Packets::WorldMessages::Normal;

		if (load >= minMaxLoad && load < maxLoad) {
			message = Packets::WorldMessages::HeavyLoad;
		}
		else if (load == maxLoad) {
			message = Packets::WorldMessages::MaxLoad;
		}
		user->send(Packets::showChannels(message));
	}
	else {
		// Hacking
		return;
	}
}

auto Worlds::channelSelect(ref_ptr_t<User> user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	world_id_t worldId = reader.get<world_id_t>();
	if (World *world = getWorld(worldId)) {
		user->setWorldId(worldId);
	}
	else {
		// Hacking
		return;
	}

	channel_id_t channelId = reader.get<int8_t>();

	user->send(Packets::channelSelect());
	World *world = m_worlds[worldId];

	if (world == nullptr) {
		// Hacking, lag, or client that hasn't been updated (e.g. in the middle of logging in)
		return;
	}

	if (Channel *channel = world->getChannel(channelId)) {
		user->setChannel(channelId);
		Characters::showCharacters(user);
	}
	else {
		user->send(Packets::channelOffline());
	}
}

auto Worlds::addWorldServer(ref_ptr_t<LoginServerAcceptedSession> session) -> optional_t<world_id_t> {
	World *world = nullptr;
	for (const auto &kvp : m_worlds) {
		if (!kvp.second->isConnected()) {
			world = kvp.second;
			break;
		}
	}

	auto &server = LoginServer::getInstance();
	if (world == nullptr) {
		session->send(Packets::Interserver::noMoreWorld());
		server.log(LogType::Error, "No more worlds to assign.");
		session->disconnect();
		return {};
	}

	optional_t<world_id_t> worldId = world->getId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}

	world_id_t cached = worldId.get();
	session->setWorldId(cached);
	world->setConnected(true);
	world->setSession(session);

	session->send(Packets::Interserver::connect(world));

	server.log(LogType::ServerConnect, [&](out_stream_t &log) {
		log << "World " << static_cast<int32_t>(cached);
	});

	return cached;
}

auto Worlds::addChannelServer(ref_ptr_t<LoginServerAcceptedSession> session) -> optional_t<world_id_t> {
	World *validWorld = nullptr;
	for (const auto &kvp : m_worlds) {
		World *world = kvp.second;
		if (world->getChannelCount() < world->getMaxChannels() && world->isConnected()) {
			validWorld = world;
			break;
		}
	}

	if (validWorld == nullptr) {
		session->send(Packets::Interserver::connectChannel({}, {}, {}));
		LoginServer::getInstance().log(LogType::Error, "No more channels to assign.");
		session->disconnect();
		return {};
	}

	optional_t<world_id_t> worldId = validWorld->getId();
	if (!worldId.is_initialized()) {
		throw CodePathInvalidException{"!worldId.is_initialized()"};
	}

	Ip worldIp = validWorld->matchSubnet(session->getIp().get(Ip{0}));
	session->send(Packets::Interserver::connectChannel(worldId.get(), worldIp, validWorld->getPort()));
	session->disconnect();
	return worldId;
}

auto Worlds::send(world_id_t id, const PacketBuilder &builder) -> void {
	if (World *world = getWorld(id)) {
		if (world->isConnected()) {
			world->send(builder);
		}
	}
}

auto Worlds::send(const vector_t<world_id_t> &worlds, const PacketBuilder &builder) -> void {
	for (const auto &worldId : worlds) {
		auto kvp = m_worlds.find(worldId);
		if (kvp != std::end(m_worlds) && kvp->second->isConnected()) {
			kvp->second->send(builder);
		}
	}
}

auto Worlds::send(const PacketBuilder &builder) -> void {
	for (const auto &kvp : m_worlds) {
		if (kvp.second->isConnected()) {
			kvp.second->send(builder);
		}
	}
}

auto Worlds::runFunction(function_t<bool (World *)> func) -> void {
	for (const auto &kvp : m_worlds) {
		if (func(kvp.second)) {
			break;
		}
	}
}

auto Worlds::calculatePlayerLoad(World *world) -> void {
	world->setPlayerLoad(0);
	world->runChannelFunction([&world](Channel *channel) {
		world->setPlayerLoad(world->getPlayerLoad() + channel->getPopulation());
	});
}

auto Worlds::getWorld(world_id_t id) -> World * {
	auto kvp = m_worlds.find(id);
	return kvp != std::end(m_worlds) ? kvp->second : nullptr;
}

auto Worlds::setEventMessages(const string_t &message) -> void {
	for (const auto &kvp : m_worlds) {
		kvp.second->setEventMessage(message);
	}
}

}
}