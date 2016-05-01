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

#include "Common/AbstractServer.hpp"
#include "Common/FinalizationPool.hpp"
#include "Common/Ip.hpp"
#include "Common/RatesConfig.hpp"
#include "Common/Types.hpp"
#include "Common/WorldConfig.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/LoginServerSession.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/WorldServerAcceptedSession.hpp"
#include <string>

namespace Vana {
	class PacketBuilder;

	namespace WorldServer {
		class WorldServer final : public AbstractServer {
			SINGLETON(WorldServer);
		public:
			auto shutdown() -> void override;
			auto establishedLoginConnection(world_id_t worldId, port_t port, const WorldConfig &conf) -> void;
			auto rehashConfig(const WorldConfig &config) -> void;
			auto setScrollingHeader(const string_t &message) -> void;
			auto setRates(const RatesConfig &rates) -> void;
			auto resetRates(int32_t flags) -> void;
			auto getPlayerDataProvider() -> PlayerDataProvider &;
			auto getChannels() -> Channels &;
			auto isConnected() const -> bool;
			auto getWorldId() const -> world_id_t;
			auto makeChannelPort(channel_id_t channelId) const -> port_t;
			auto getConfig() -> const WorldConfig &;
			auto sendLogin(const PacketBuilder &builder) -> void;
			auto onConnectToLogin(ref_ptr_t<LoginServerSession> connection) -> void;
			auto onDisconnectFromLogin() -> void;
			auto finalizeServerSession(ref_ptr_t<WorldServerAcceptedSession> session) -> void;
		protected:
			auto listen() -> void;
			auto loadData() -> Result override;
			auto makeLogIdentifier() const -> opt_string_t override;
			auto getLogPrefix() const -> string_t override;
		private:
			world_id_t m_worldId = -1;
			port_t m_port = 0;
			WorldConfig m_config;
			RatesConfig m_defaultRates;
			ref_ptr_t<LoginServerSession> m_loginSession;
			PlayerDataProvider m_playerDataProvider;
			Channels m_channels;
			FinalizationPool<WorldServerAcceptedSession> m_sessionPool;
		};
	}
}