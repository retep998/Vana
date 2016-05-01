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
		class LoginServerAcceptedSession;

		class World {
			NONCOPYABLE(World);
		public:
			World() = default;

			auto setConnected(bool connected) -> void;
			auto setId(world_id_t id) -> void;
			auto setPort(port_t port) -> void;
			auto setPlayerLoad(int32_t load) -> void;
			auto setSession(ref_ptr_t<LoginServerAcceptedSession> session) -> void;
			auto setConfiguration(const WorldConfig &config) -> void;
			auto setEventMessage(const string_t &message) -> void;
			auto runChannelFunction(function_t<void (Channel *)> func) -> void;
			auto clearChannels() -> void;
			auto removeChannel(channel_id_t id) -> void;
			auto addChannel(channel_id_t id, Channel *channel) -> void;
			auto send(const PacketBuilder &builder) -> void;

			auto isConnected() const -> bool;
			auto getId() const -> optional_t<world_id_t>;
			auto getRibbon() const -> int8_t;
			auto getPort() const -> port_t;
			auto getRandomChannel() const -> channel_id_t;
			auto getMaxChannels() const -> channel_id_t;
			auto getPlayerLoad() const -> int32_t;
			auto getMaxPlayerLoad() const -> int32_t;
			auto matchSubnet(const Ip &test) -> Ip;
			auto getChannelCount() const -> channel_id_t;
			auto getName() const -> string_t;
			auto getEventMessage() const -> string_t;
			auto getChannel(channel_id_t id) -> Channel *;
			auto getConfig() const -> const WorldConfig &;
		private:
			bool m_connected = false;
			optional_t<world_id_t> m_id;
			port_t m_port = 0;
			int32_t m_playerLoad = 0;
			ref_ptr_t<LoginServerAcceptedSession> m_session;
			WorldConfig m_config;
			hash_map_t<channel_id_t, ref_ptr_t<Channel>> m_channels;
		};
	}
}