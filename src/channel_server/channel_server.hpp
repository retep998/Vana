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
#include "common/config/world.hpp"
#include "common/data/provider/beauty.hpp"
#include "common/data/provider/buff.hpp"
#include "common/data/provider/curse.hpp"
#include "common/data/provider/drop.hpp"
#include "common/data/provider/equip.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/map.hpp"
#include "common/data/provider/mob.hpp"
#include "common/data/provider/npc.hpp"
#include "common/data/provider/quest.hpp"
#include "common/data/provider/reactor.hpp"
#include "common/data/provider/script.hpp"
#include "common/data/provider/skill.hpp"
#include "common/data/provider/shop.hpp"
#include "common/data/provider/valid_char.hpp"
#include "common/ip.hpp"
#include "common/types.hpp"
#include "common/util/finalization_pool.hpp"
#include "channel_server/event_data_provider.hpp"
#include "channel_server/instances.hpp"
#include "channel_server/login_server_session.hpp"
#include "channel_server/map_factory.hpp"
#include "channel_server/maple_tvs.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/trades.hpp"
#include "channel_server/world_server_session.hpp"
#include <string>
#include <vector>

namespace vana {
	class packet_builder;
	namespace config {
		struct rates;
	}

	namespace channel_server {
		class map;

		class channel_server final : public abstract_server {
			SINGLETON(channel_server);
		public:
			auto shutdown() -> void override;
			auto connect_to_world(game_world_id world_id, connection_port port, const ip &ip) -> result;
			auto established_world_connection(game_channel_id channel_id, connection_port port, const config::world &config) -> void;

			// TODO FIXME api
			// Eyeball these for potential refactoring - they involve world<->channel operations and I don't want to dig into that now
			auto set_scrolling_header(const string &message) -> void;
			auto modify_rate(int32_t rate_type, int32_t new_value) -> void;
			auto set_config(const config::world &config) -> void;
			auto set_rates(const config::rates &rates) -> void;

			auto reload_data(const string &args) -> void;

			auto get_valid_char_data_provider() const -> const data::provider::valid_char &;
			auto get_equip_data_provider() const -> const data::provider::equip &;
			auto get_curse_data_provider() const -> const data::provider::curse &;
			auto get_npc_data_provider() const -> const data::provider::npc &;
			auto get_mob_data_provider() const -> const data::provider::mob &;
			auto get_beauty_data_provider() const -> const data::provider::beauty &;
			auto get_drop_data_provider() const -> const data::provider::drop &;
			auto get_skill_data_provider() const -> const data::provider::skill &;
			auto get_shop_data_provider() const -> const data::provider::shop &;
			auto get_script_data_provider() const -> const data::provider::script &;
			auto get_reactor_data_provider() const -> const data::provider::reactor &;
			auto get_item_data_provider() const -> const data::provider::item &;
			auto get_quest_data_provider() const -> const data::provider::quest &;
			auto get_buff_data_provider() const -> const data::provider::buff &;
			auto get_event_data_provider() const -> const event_data_provider &;
			auto get_map_data_provider() -> data::provider::map &;
			auto get_player_data_provider() -> player_data_provider &;
			auto get_map_factory() const -> map_factory &;
			auto get_trades() -> trades &;
			auto get_maple_tvs() -> maple_tvs &;
			auto get_instances() -> instances &;

			auto get_map(int32_t map_id) -> map *;
			auto unload_map(int32_t map_id) -> void;

			auto is_connected() const -> bool;
			auto get_world_id() const -> game_world_id;
			auto get_channel_id() const -> game_channel_id;
			auto get_online_id() const -> int32_t;
			auto get_config() const -> const config::world &;
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
			config::world m_config;
			ref_ptr<world_server_session> m_world_connection;
			ref_ptr<login_server_session> m_login_connection;
			vana::util::finalization_pool<player> m_session_pool;

			data::provider::valid_char m_valid_char_data_provider;
			data::provider::equip m_equip_data_provider;
			data::provider::curse m_curse_data_provider;
			data::provider::npc m_npc_data_provider;
			data::provider::mob m_mob_data_provider;
			data::provider::beauty m_beauty_data_provider;
			data::provider::drop m_drop_data_provider;
			data::provider::skill m_skill_data_provider;
			data::provider::shop m_shop_data_provider;
			data::provider::script m_script_data_provider;
			data::provider::reactor m_reactor_data_provider;
			data::provider::item m_item_data_provider;
			data::provider::quest m_quest_data_provider;
			data::provider::buff m_buff_data_provider;
			data::provider::map m_map_data_provider;
			event_data_provider m_event_data_provider;
			player_data_provider m_player_data_provider;
			map_factory m_map_factory;
			trades m_trades;
			maple_tvs m_maple_tvs;
			instances m_instances;
		};
	}
}