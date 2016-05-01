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
#include "World.hpp"
#include "LoginServer/LoginServerAcceptedSession.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Randomizer.hpp"

namespace Vana {
namespace LoginServer {

auto World::setConnected(bool connected) -> void {
	m_connected = connected;
	if (!connected) {
		m_session.reset();
	}
}

auto World::setId(world_id_t id) -> void {
	m_id = id;
}

auto World::setPort(port_t port) -> void {
	m_port = port;
}

auto World::setPlayerLoad(int32_t load) -> void {
	m_playerLoad = load;
}

auto World::setSession(ref_ptr_t<LoginServerAcceptedSession> session) -> void {
	m_session = session;
}

auto World::setConfiguration(const WorldConfig &config) -> void {
	m_config = config;
}

auto World::setEventMessage(const string_t &message) -> void {
	m_config.eventMessage = message;
}

auto World::runChannelFunction(function_t<void (Channel *)> func) -> void {
	for (const auto &kvp : m_channels) {
		func(kvp.second.get());
	}
}

auto World::clearChannels() -> void {
	m_channels.clear();
}

auto World::removeChannel(channel_id_t id) -> void {
	m_channels.erase(id);
}

auto World::addChannel(channel_id_t id, Channel *channel) -> void {
	m_channels[id].reset(channel);
}

auto World::send(const PacketBuilder &builder) -> void {
	m_session->send(builder);
}

auto World::isConnected() const -> bool {
	return m_connected;
}

auto World::getId() const -> optional_t<world_id_t> {
	return m_id;
}

auto World::getRibbon() const -> int8_t {
	return m_config.ribbon;
}

auto World::getPort() const -> port_t {
	return m_port;
}

auto World::getRandomChannel() const -> channel_id_t {
	return Randomizer::select(m_channels)->first;
}

auto World::getMaxChannels() const -> channel_id_t {
	return m_config.maxChannels;
}

auto World::getPlayerLoad() const -> int32_t {
	return m_playerLoad;
}

auto World::getMaxPlayerLoad() const -> int32_t {
	return m_config.maxPlayerLoad;
}

auto World::matchSubnet(const Ip &test) -> Ip {
	return m_session->matchSubnet(test);
}

auto World::getChannelCount() const -> channel_id_t {
	return static_cast<channel_id_t>(m_channels.size());
}

auto World::getName() const -> string_t {
	return m_config.name;
}

auto World::getEventMessage() const -> string_t {
	return m_config.eventMessage;
}

auto World::getChannel(channel_id_t id) -> Channel * {
	return m_channels.find(id) != std::end(m_channels) ?
		m_channels[id].get() :
		nullptr;
}

auto World::getConfig() const -> const WorldConfig & {
	return m_config;
}

}
}