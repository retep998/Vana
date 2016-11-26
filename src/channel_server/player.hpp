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

#include "common/charge_or_stationary_skill_data.hpp"
#include "common/data/provider/skill.hpp"
#include "common/packet_handler.hpp"
#include "common/tausworthe_generator.hpp"
#include "common/timer/container_holder.hpp"
#include "channel_server/movable_life.hpp"
#include "channel_server/npc.hpp"
#include "channel_server/player_active_buffs.hpp"
#include "channel_server/player_buddy_list.hpp"
#include "channel_server/player_inventory.hpp"
#include "channel_server/player_monster_book.hpp"
#include "channel_server/player_mounts.hpp"
#include "channel_server/player_pets.hpp"
#include "channel_server/player_quests.hpp"
#include "channel_server/player_skills.hpp"
#include "channel_server/player_stats.hpp"
#include "channel_server/player_storage.hpp"
#include "channel_server/player_summons.hpp"
#include "channel_server/player_variables.hpp"
#include <ctime>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace vana {
	class packet_builder;
	class packet_reader;
	struct split_packet_builder;
	namespace data {
		namespace type {
			struct portal_info;
		}
	}

	namespace channel_server {
		class instance;
		class map;
		class party;

		class player : public packet_handler, public enable_shared<player>, public vana::timer::container_holder, public movable_life {
			NONCOPYABLE(player);
		public:
			player();

			auto set_gm_chat(bool chat) -> void { m_gm_chat = chat; }
			auto set_save_on_dc(bool save) -> void { m_save_on_dc = save; }
			auto set_trading(bool state) -> void { m_trade_state = state; }
			auto set_changing_channel(bool v) -> void { m_changing_channel = v; }
			auto set_stalking(bool stalk) -> void { m_stalking = stalk; }
			auto set_skin(game_skin_id id) -> void;
			auto set_fall_counter(int8_t falls) -> void { m_fall_counter = falls; }
			auto set_map_chair(game_seat_id s) -> void { m_map_chair = s; }
			auto set_face(game_face_id id) -> void;
			auto set_hair(game_hair_id id) -> void;
			auto set_map(game_map_id map_id, const data::type::portal_info * const portal = nullptr, bool is_instance = false) -> void;
			auto set_map(game_map_id map_id, game_portal_id portal_id, const point &pos) -> void;
			auto set_buddy_list_size(uint8_t size) -> void;
			auto set_connection_time(int64_t newtime) -> void { m_online_time = newtime; }
			auto set_trade_id(game_trade_id id) -> void { m_trade_id = id; }
			auto set_shop(game_shop_id shop_id) -> void { m_shop = shop_id; }
			auto set_chair(game_item_id chair) -> void { m_chair = chair; }
			auto set_item_effect(game_item_id effect) -> void { m_item_effect = effect; }
			auto set_chalkboard(const string &msg) -> void { m_chalkboard = msg; }
			auto set_charge_or_stationary_skill(const charge_or_stationary_skill_data &info) -> void { m_info = info; }
			auto set_npc(npc *npc) -> void { m_npc.reset(npc); }
			auto set_party(party *party) -> void { m_party = party; }
			auto set_follow(ref_ptr<player> target) -> void { m_follow = target; }
			auto set_instance(instance *inst) -> void { m_instance = inst; }
			auto parse_transfer_packet(packet_reader &reader) -> void;

			auto is_gm() const -> bool { return m_gm_level > 0; }
			auto is_gm_chat() const -> bool { return m_gm_chat; }
			auto is_admin() const -> bool { return m_admin; }
			auto is_changing_channel() const -> bool { return m_changing_channel; }
			auto is_trading() const -> bool { return m_trade_state; }
			auto is_disconnecting() const -> bool { return m_disconnecting; }
			auto is_stalking() const -> bool { return m_stalking; }
			auto has_gm_equip() const -> bool;
			auto is_using_gm_hide() const -> bool;
			auto has_gm_benefits() const -> bool;
			auto has_charge_or_stationary_skill() const -> bool { return m_info.skill_id != 0; }
			auto get_world_id() const -> game_world_id { return m_world_id; }
			auto get_gender() const -> game_gender_id { return m_gender; }
			auto get_skin() const -> game_skin_id { return m_skin; }
			auto get_map_pos() const -> game_portal_id { return m_map_pos; }
			auto get_fall_counter() const -> int8_t { return m_fall_counter; }
			auto get_buddy_list_size() const -> uint8_t { return m_buddylist_size; }
			auto get_portal_count(bool add = false) -> game_portal_count;
			auto get_map_chair() const -> game_seat_id { return m_map_chair; }
			auto get_id() const -> game_player_id { return m_id; }
			auto get_account_id() const -> game_account_id { return m_account_id; }
			auto get_face() const -> game_face_id { return m_face; }
			auto get_hair() const -> game_hair_id { return m_hair; }
			auto get_map_id() const -> game_map_id { return m_map; }
			auto get_last_map_id() const -> game_map_id { return m_last_map; }
			auto get_shop() const -> game_shop_id { return m_shop; }
			auto get_chair() const -> game_item_id { return m_chair; }
			auto get_item_effect() const -> game_item_id { return m_item_effect; }
			auto get_gm_level() const -> int32_t { return m_gm_level; }
			auto get_trade_id() const -> game_trade_id { return m_trade_id; }
			auto get_connection_time() const -> int64_t { return m_online_time; }
			auto get_connected_time() const -> int64_t { return time(nullptr) - m_online_time; }
			auto get_chalkboard() const -> string { return m_chalkboard; }
			auto get_medal_name() -> string;
			auto get_name() const -> string { return m_name; }
			auto get_charge_or_stationary_skill() const -> charge_or_stationary_skill_data { return m_info; }
			auto get_transfer_packet() const -> packet_builder;

			auto get_map() const -> map *;
			auto get_follow() const -> ref_ptr<player> { return m_follow; }
			auto get_npc() const -> npc * { return m_npc.get(); }
			auto get_party() const -> party * { return m_party; }
			auto get_instance() const -> instance * { return m_instance; }
			auto get_active_buffs() const -> player_active_buffs * { return m_active_buffs.get(); }
			auto get_buddy_list() const -> player_buddy_list * { return m_buddy_list.get(); }
			auto get_inventory() const -> player_inventory * { return m_inventory.get(); }
			auto get_monster_book() const -> player_monster_book * { return m_monster_book.get(); }
			auto get_mounts() const -> player_mounts * { return m_mounts.get(); }
			auto get_pets() const -> player_pets * { return m_pets.get(); }
			auto get_quests() const -> player_quests * { return m_quests.get(); }
			auto get_skills() const -> player_skills * { return m_skills.get(); }
			auto get_stats() const -> player_stats * { return m_stats.get(); }
			auto get_storage() const -> player_storage * { return m_storage.get(); }
			auto get_summons() const -> player_summons * { return m_summons.get(); }
			auto get_variables() const -> player_variables * { return m_variables.get(); }
			auto get_rand_stream() const -> tausworthe_generator * { return m_rand_stream.get(); }
			auto get_timer_container() const -> ref_ptr<vana::timer::container> { return get_timers(); }

			auto add_used_portal(game_portal_id portal_id) -> void { m_used_portals.insert(portal_id); }
			auto used_portal(game_portal_id portal_id) const -> bool { return m_used_portals.find(portal_id) != std::end(m_used_portals); }

			auto change_channel(game_channel_id channel) -> void;
			auto save_all(bool save_cooldowns = false) -> void;
			auto set_online(bool online) -> void;
			auto set_level_date() -> void;
			auto accept_death(bool wheel) -> void;
			auto initialize_rng(packet_builder &builder) -> void;

			auto send(const packet_builder &builder) -> void;
			auto send(const split_packet_builder &builder) -> void;
			auto send_map(const packet_builder &builder, bool exclude_self = false) -> void;
			auto send_map(const split_packet_builder &builder) -> void;
		protected:
			auto handle(packet_reader &reader) -> result override;
			auto on_disconnect() -> void override;
		private:
			auto player_connect(packet_reader &reader) -> void;
			auto change_key(packet_reader &reader) -> void;
			auto change_skill_macros(packet_reader &reader) -> void;
			auto save_stats() -> void;
			auto internal_set_map(game_map_id map_id, game_portal_id portal_id, const point &pos, bool from_position) -> void;

			bool m_trade_state = false;
			bool m_save_on_dc = true;
			bool m_is_connect = false;
			bool m_changing_channel = false;
			bool m_admin = false;
			bool m_gm_chat = false;
			bool m_disconnecting = false;
			bool m_stalking = false;
			game_world_id m_world_id = -1;
			game_portal_id m_map_pos = -1;
			game_gender_id m_gender = -1;
			game_skin_id m_skin = 0;
			int8_t m_fall_counter = 0;
			uint8_t m_buddylist_size = 0;
			game_portal_count m_portal_count = 0;
			game_seat_id m_map_chair = 0;
			game_player_id m_id = 0;
			game_account_id m_account_id = 0;
			game_face_id m_face = 0;
			game_hair_id m_hair = 0;
			game_map_id m_map = 0;
			game_map_id m_last_map = 0;
			game_shop_id m_shop = 0;
			game_item_id m_item_effect = 0;
			game_item_id m_chair = 0;
			int32_t m_gm_level = 0;
			game_trade_id m_trade_id = 0;
			int64_t m_online_time = 0;
			instance *m_instance = nullptr;
			party *m_party = nullptr;
			string m_chalkboard;
			string m_name;
			charge_or_stationary_skill_data m_info;
			ref_ptr<player> m_follow = nullptr;
			owned_ptr<npc> m_npc;
			owned_ptr<player_active_buffs> m_active_buffs;
			owned_ptr<player_buddy_list> m_buddy_list;
			owned_ptr<player_inventory> m_inventory;
			owned_ptr<player_monster_book> m_monster_book;
			owned_ptr<player_mounts> m_mounts;
			owned_ptr<player_pets> m_pets;
			owned_ptr<player_quests> m_quests;
			owned_ptr<player_skills> m_skills;
			owned_ptr<player_stats> m_stats;
			owned_ptr<player_storage> m_storage;
			owned_ptr<player_summons> m_summons;
			owned_ptr<player_variables> m_variables;
			owned_ptr<tausworthe_generator> m_rand_stream;
			hash_set<game_portal_id> m_used_portals;
		};
	}
}