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
#include "Common/BeautyDataProvider.hpp"
#include "Common/BuffDataProvider.hpp"
#include "Common/CurseDataProvider.hpp"
#include "Common/DropDataProvider.hpp"
#include "Common/EquipDataProvider.hpp"
#include "Common/FinalizationPool.hpp"
#include "Common/Ip.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/MobDataProvider.hpp"
#include "Common/NpcDataProvider.hpp"
#include "Common/QuestDataProvider.hpp"
#include "Common/ReactorDataProvider.hpp"
#include "Common/ScriptDataProvider.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/ShopDataProvider.hpp"
#include "Common/Types.hpp"
#include "Common/ValidCharDataProvider.hpp"
#include "Common/WorldConfig.hpp"
#include "ChannelServer/EventDataProvider.hpp"
#include "ChannelServer/Instances.hpp"
#include "ChannelServer/LoginServerSession.hpp"
#include "ChannelServer/MapDataProvider.hpp"
#include "ChannelServer/MapleTvs.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/Trades.hpp"
#include "ChannelServer/WorldServerSession.hpp"
#include <string>
#include <vector>

namespace Vana {
	class PacketBuilder;
	struct RatesConfig;

	namespace ChannelServer {
		class Map;

		class ChannelServer final : public AbstractServer {
			SINGLETON(ChannelServer);
		public:
			auto shutdown() -> void override;
			auto connectToWorld(world_id_t worldId, port_t port, const Ip &ip) -> Result;
			auto establishedWorldConnection(channel_id_t channelId, port_t port, const WorldConfig &config) -> void;

			// TODO FIXME api
			// Eyeball these for potential refactoring - they involve world<->channel operations and I don't want to dig into that now
			auto setScrollingHeader(const string_t &message) -> void;
			auto modifyRate(int32_t rateType, int32_t newValue) -> void;
			auto setConfig(const WorldConfig &config) -> void;
			auto setRates(const RatesConfig &rates) -> void;

			auto reloadData(const string_t &args) -> void;

			auto getValidCharDataProvider() const -> const ValidCharDataProvider &;
			auto getEquipDataProvider() const -> const EquipDataProvider &;
			auto getCurseDataProvider() const -> const CurseDataProvider &;
			auto getNpcDataProvider() const -> const NpcDataProvider &;
			auto getMobDataProvider() const -> const MobDataProvider &;
			auto getBeautyDataProvider() const -> const BeautyDataProvider &;
			auto getDropDataProvider() const -> const DropDataProvider &;
			auto getSkillDataProvider() const -> const SkillDataProvider &;
			auto getShopDataProvider() const -> const ShopDataProvider &;
			auto getScriptDataProvider() const -> const ScriptDataProvider &;
			auto getReactorDataProvider() const -> const ReactorDataProvider &;
			auto getItemDataProvider() const -> const ItemDataProvider &;
			auto getQuestDataProvider() const -> const QuestDataProvider &;
			auto getBuffDataProvider() const -> const BuffDataProvider &;
			auto getEventDataProvider() const -> const EventDataProvider &;
			auto getMapDataProvider() const -> const MapDataProvider &;
			auto getPlayerDataProvider() -> PlayerDataProvider &;
			auto getTrades() -> Trades &;
			auto getMapleTvs() -> MapleTvs &;
			auto getInstances() -> Instances &;

			auto getMap(int32_t mapId) -> Map *;
			auto unloadMap(int32_t mapId) -> void;

			auto isConnected() const -> bool;
			auto getWorldId() const -> world_id_t;
			auto getChannelId() const -> channel_id_t;
			auto getOnlineId() const -> int32_t;
			auto getConfig() const -> const WorldConfig &;
			auto sendWorld(const PacketBuilder &builder) -> void;
			auto onConnectToLogin(ref_ptr_t<LoginServerSession> session) -> void;
			auto onDisconnectFromLogin() -> void;
			auto onConnectToWorld(ref_ptr_t<WorldServerSession> session) -> void;
			auto onDisconnectFromWorld() -> void;
			auto finalizePlayer(ref_ptr_t<Player> session) -> void;
		protected:
			auto loadData() -> Result override;
			auto listen() -> void;
			auto makeLogIdentifier() const -> opt_string_t override;
			auto getLogPrefix() const -> string_t override;
		private:
			world_id_t m_worldId = -1;
			channel_id_t m_channelId = -1;
			port_t m_worldPort = 0;
			port_t m_port = 0;
			Ip m_worldIp;
			WorldConfig m_config;
			ref_ptr_t<WorldServerSession> m_worldConnection;
			ref_ptr_t<LoginServerSession> m_loginConnection;
			FinalizationPool<Player> m_sessionPool;

			ValidCharDataProvider m_validCharDataProvider;
			EquipDataProvider m_equipDataProvider;
			CurseDataProvider m_curseDataProvider;
			NpcDataProvider m_npcDataProvider;
			MobDataProvider m_mobDataProvider;
			BeautyDataProvider m_beautyDataProvider;
			DropDataProvider m_dropDataProvider;
			SkillDataProvider m_skillDataProvider;
			ShopDataProvider m_shopDataProvider;
			ScriptDataProvider m_scriptDataProvider;
			ReactorDataProvider m_reactorDataProvider;
			ItemDataProvider m_itemDataProvider;
			QuestDataProvider m_questDataProvider;
			BuffDataProvider m_buffDataProvider;
			EventDataProvider m_eventDataProvider;
			MapDataProvider m_mapDataProvider;
			PlayerDataProvider m_playerDataProvider;
			Trades m_trades;
			MapleTvs m_mapleTvs;
			Instances m_instances;
		};
	}
}