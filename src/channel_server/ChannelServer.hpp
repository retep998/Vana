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

#include "common/abstract_server.hpp"
#include "common/beauty_data_provider.hpp"
#include "common/buff_data_provider.hpp"
#include "common/curse_data_provider.hpp"
#include "common/drop_data_provider.hpp"
#include "common/equip_data_provider.hpp"
#include "common/finalization_pool.hpp"
#include "common/ip.hpp"
#include "common/item_data_provider.hpp"
#include "common/mob_data_provider.hpp"
#include "common/npc_data_provider.hpp"
#include "common/quest_data_provider.hpp"
#include "common/reactor_data_provider.hpp"
#include "common/script_data_provider.hpp"
#include "common/skill_data_provider.hpp"
#include "common/shop_data_provider.hpp"
#include "common/types.hpp"
#include "common/valid_char_data_provider.hpp"
#include "common/world_config.hpp"
#include "channel_server/EventDataProvider.hpp"
#include "channel_server/Instances.hpp"
#include "channel_server/LoginServerSession.hpp"
#include "channel_server/MapDataProvider.hpp"
#include "channel_server/MapleTvs.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/Trades.hpp"
#include "channel_server/WorldServerSession.hpp"
#include <string>
#include <vector>

namespace vana {
	class packet_builder;
	struct rates_config;

	namespace channel_server {
		class map;

		class channel_server final : public abstract_server {
			SINGLETON(channel_server);
		public:
			auto shutdown() -> void override;
			auto connect_to_world(game_world_id world_id, connection_port port, const ip &ip) -> result;
			auto established_world_connection(game_channel_id channel_id, connection_port port, const world_config &config) -> void;

			// TODO FIXME api
			// Eyeball these for potential refactoring - they involve world<->channel operations and I don't want to dig into that now
			auto set_scrolling_header(const string &message) -> void;
			auto modify_rate(int32_t rate_type, int32_t new_value) -> void;
			auto set_config(const world_config &config) -> void;
			auto set_rates(const rates_config &rates) -> void;

			auto reload_data(const string &args) -> void;

			auto get_valid_char_data_provider() const -> const valid_char_data_provider &;
			auto get_equip_data_provider() const -> const equip_data_provider &;
			auto get_curse_data_provider() const -> const curse_data_provider &;
			auto get_npc_data_provider() const -> const npc_data_provider &;
			auto get_mob_data_provider() const -> const mob_data_provider &;
			auto get_beauty_data_provider() const -> const beauty_data_provider &;
			auto get_drop_data_provider() const -> const drop_data_provider &;
			auto get_skill_data_provider() const -> const skill_data_provider &;
			auto get_shop_data_provider() const -> const shop_data_provider &;
			auto get_script_data_provider() const -> const script_data_provider &;
			auto get_reactor_data_provider() const -> const reactor_data_provider &;
			auto get_item_data_provider() const -> const item_data_provider &;
			auto get_quest_data_provider() const -> const quest_data_provider &;
			auto get_buff_data_provider() const -> const buff_data_provider &;
			auto get_event_data_provider() const -> const event_data_provider &;
			auto get_map_data_provider() const -> const map_data_provider &;
			auto get_player_data_provider() -> player_data_provider &;
			auto get_trades() -> trades &;
			auto get_maple_tvs() -> maple_tvs &;
			auto get_instances() -> instances &;

			auto get_map(int32_t map_id) -> map *;
			auto unload_map(int32_t map_id) -> void;

			auto is_connected() const -> bool;
			auto get_world_id() const -> game_world_id;
			auto get_channel_id() const -> game_channel_id;
			auto get_online_id() const -> int32_t;
			auto get_config() const -> const world_config &;
			auto send_world(const packet_builder &builder) -> void;
			auto on_connect_to_login(ref_ptr<login_server_session> session) -> void;
			auto on_disconnect_from_login() -> void;
			auto on_connect_to_world(ref_ptr<world_server_session> session) -> void;
			auto on_disconnect_from_world() -> void;
			auto finalize_player(ref_ptr<player> session) -> void;
		protected:
			auto load_data() -> result override;
			auto listen() -> void;
			auto make_log_identifier() const -> opt_string override;
			auto get_log_prefix() const -> string override;
		private:
			game_world_id m_world_id = -1;
			game_channel_id m_channel_id = -1;
			connection_port m_world_port = 0;
			connection_port m_port = 0;
			ip m_world_ip;
			world_config m_config;
			ref_ptr<world_server_session> m_world_connection;
			ref_ptr<login_server_session> m_login_connection;
			finalization_pool<player> m_session_pool;

			valid_char_data_provider m_valid_char_data_provider;
			equip_data_provider m_equip_data_provider;
			curse_data_provider m_curse_data_provider;
			npc_data_provider m_npc_data_provider;
			mob_data_provider m_mob_data_provider;
			beauty_data_provider m_beauty_data_provider;
			drop_data_provider m_drop_data_provider;
			skill_data_provider m_skill_data_provider;
			shop_data_provider m_shop_data_provider;
			script_data_provider m_script_data_provider;
			reactor_data_provider m_reactor_data_provider;
			item_data_provider m_item_data_provider;
			quest_data_provider m_quest_data_provider;
			buff_data_provider m_buff_data_provider;
			event_data_provider m_event_data_provider;
			map_data_provider m_map_data_provider;
			player_data_provider m_player_data_provider;
			trades m_trades;
			maple_tvs m_maple_tvs;
			instances m_instances;
		};
	}
}