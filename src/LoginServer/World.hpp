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
#pragma once

#include "Common/Types.hpp"
#include "Common/WorldConfig.hpp"
#include "LoginServer/Channel.hpp"
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	class PacketBuilder;

	namespace LoginServer {
		class Channel;
		class LoginServerAcceptConnection;

		class World {
			NONCOPYABLE(World);
		public:
			World() = default;

			auto setConnected(bool connected) -> void { m_connected = connected; }
			auto setId(world_id_t id) -> void { m_id = id; }
			auto setPort(port_t port) -> void { m_port = port; }
			auto setPlayerLoad(int32_t load) -> void { m_playerLoad = load; }
			auto setConnection(LoginServerAcceptConnection *connection) -> void { m_connection = connection; }
			auto setConfiguration(const WorldConfig &config) -> void { m_config = config; }
			auto setEventMessage(const string_t &message) -> void { m_config.eventMessage = message; }
			auto runChannelFunction(function_t<void (Channel *)> func) -> void;
			auto clearChannels() -> void { m_channels.clear(); }
			auto removeChannel(channel_id_t id) -> void { m_channels.erase(id); }
			auto addChannel(channel_id_t id, Channel *channel) -> void { m_channels[id].reset(channel); }
			auto send(const PacketBuilder &builder) -> void;

			auto isConnected() const -> bool { return m_connected; }
			auto getId() const -> world_id_t { return m_id; }
			auto getRibbon() const -> int8_t { return m_config.ribbon; }
			auto getPort() const -> port_t { return m_port; }
			auto getRandomChannel() const -> channel_id_t;
			auto getMaxChannels() const -> channel_id_t { return m_config.maxChannels; }
			auto getPlayerLoad() const -> int32_t { return m_playerLoad; }
			auto getMaxPlayerLoad() const -> int32_t { return m_config.maxPlayerLoad; }
			auto matchSubnet(const Ip &test) -> Ip;
			auto getChannelCount() const -> channel_id_t { return static_cast<channel_id_t>(m_channels.size()); }
			auto getName() const -> string_t { return m_config.name; }
			auto getEventMessage() const -> string_t { return m_config.eventMessage; }
			auto getChannel(channel_id_t id) -> Channel * { return m_channels.find(id) != std::end(m_channels) ? m_channels[id].get() : nullptr; }
			auto getConfig() const -> const WorldConfig & { return m_config; }
		private:
			bool m_connected = false;
			world_id_t m_id = -1;
			port_t m_port = 0;
			int32_t m_playerLoad = 0;
			LoginServerAcceptConnection *m_connection = nullptr;
			WorldConfig m_config;
			hash_map_t<channel_id_t, ref_ptr_t<Channel>> m_channels;
		};
	}
}