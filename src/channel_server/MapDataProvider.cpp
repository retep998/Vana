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
#include "MapDataProvider.hpp"
#include "common/database_temp.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/initialize_common.hpp"
#include "common/string_utilities.hpp"
#include "channel_server/Map.hpp"
#include "channel_server/MapleTvs.hpp"
#include <utility>

namespace vana {
namespace channel_server {

auto map_data_provider::get_map(game_map_id map_id) -> map * {
	if (m_maps.find(map_id) != std::end(m_maps)) {
		return m_maps[map_id];
	}
	else {
		map *map = nullptr;
		load_map(map_id, map);
		return map;
	}
}

auto map_data_provider::unload_map(game_map_id map_id) -> void {
	auto iter = m_maps.find(map_id);
	if (iter != std::end(m_maps)) {
		delete iter->second;
		m_maps.erase(iter);
	}
}

auto map_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Continents... ";

	m_continents.clear();
	int8_t map_cluster;
	int8_t continent;

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_continent_data"));

	for (const auto &row : rs) {
		map_cluster = row.get<int8_t>("map_cluster");
		continent = row.get<int8_t>("continent");

		m_continents.emplace(map_cluster, continent);
	}

	std::cout << "DONE" << std::endl;
}

auto map_data_provider::load_map(game_map_id map_id, map *&map) -> void {
	owned_lock<mutex> l{m_load_mutex};

	game_map_id check_map = load_map_data(map_id, map);
	if (check_map != -1) {
		load_seats(map, check_map);
		load_portals(map, check_map);
		load_map_life(map, check_map);
		load_footholds(map, check_map);
		load_map_time_mob(map);
	}
}

auto map_data_provider::load_map_data(game_map_id map_id, map *&data) -> game_map_id {
	game_map_id link = 0;

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_data") << " WHERE mapid = :map",
		soci::use(map_id, "map"));

	for (const auto &row : rs) {
		ref_ptr<map_info> info = make_ref_ptr<map_info>();
		link = row.get<game_map_id>("link");
		info->link = link;

		utilities::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "town") info->town = true;
			else if (cmp == "clock") info->clock = true;
			else if (cmp == "swim") info->swim = true;
			else if (cmp == "fly") info->fly = true;
			else if (cmp == "everlast") info->everlast = true;
			else if (cmp == "no_party_leader_pass") info->no_leader_pass = true;
			else if (cmp == "shop") info->shop = true;
			else if (cmp == "scroll_disable") info->scroll_disable = true;
			else if (cmp == "shuffle_reactors") info->shuffle_reactors = true;
		});

		utilities::str::run_flags(row.get<opt_string>("field_type"), [&info](const string &cmp) {
			if (cmp == "force_map_equip") info->force_map_equip = true;
		});

		utilities::str::run_flags(row.get<opt_string>("field_limitations"), [&info](const string &cmp) {
			field_limit *limitations = &info->limitations;
			if (cmp == "jump") limitations->jump = true;
			else if (cmp == "movement_skills") limitations->movement_skills = true;
			else if (cmp == "summoning_bag") limitations->summoning_bag = true;
			else if (cmp == "mystic_door") limitations->mystic_door = true;
			else if (cmp == "channel_switching") limitations->channel_switching = true;
			else if (cmp == "regular_exp_loss") limitations->regular_exp_loss = true;
			else if (cmp == "vip_rock") limitations->vip_rock = true;
			else if (cmp == "minigames") limitations->minigames = true;
			else if (cmp == "mount") limitations->mount = true;
			else if (cmp == "potion_use") limitations->potion_use = true;
			else if (cmp == "drop_down") limitations->drop_down = true;
			else if (cmp == "chalkboard") limitations->chalkboard = true;
		});

		info->continent = get_continent(map_id).get(0);
		info->return_map = row.get<game_map_id>("return_map");
		info->forced_return = row.get<game_map_id>("forced_return_map");
		info->spawn_rate = row.get<double>("mob_rate");
		info->default_music = row.get<string>("default_bgm");
		info->dimensions = rect{
			point{row.get<game_coord>("map_ltx"), row.get<game_coord>("map_lty")},
			point{row.get<game_coord>("map_rbx"), row.get<game_coord>("map_rby")}
		};
		info->shuffle_name = row.get<string>("shuffle_name");
		info->regular_hp_decrease = row.get<uint8_t>("decrease_hp");
		info->traction = row.get<double>("default_traction");
		info->regen_rate = row.get<int8_t>("regen_rate");
		info->min_level = row.get<game_player_level>("min_level_limit");
		info->time_limit = row.get<int32_t>("time_limit");
		info->protect_item = row.get<game_item_id>("protect_item");
		info->damage_per_second = row.get<game_damage>("damage_per_second");
		info->ship_kind = row.get<int8_t>("ship_kind");

		data = new map{info, map_id};
	}

	m_maps[map_id] = data;
	if (data == nullptr) {
		return -1;
	}
	return (link == 0 ? map_id : link);
}

auto map_data_provider::load_seats(map *map, game_map_id link) -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_seats") << " WHERE mapid = :map",
		soci::use(link, "map"));

	for (const auto &row : rs) {
		seat_info chair;
		game_seat_id id = row.get<game_seat_id>("seatid");
		chair.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		chair.id = id;

		map->add_seat(chair);
	}
}

auto map_data_provider::load_portals(map *map, game_map_id link) -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_portals") << " WHERE mapid = :map",
		soci::use(link, "map"));

	for (const auto &row : rs) {
		portal_info portal;
		utilities::str::run_flags(row.get<opt_string>("flags"), [&portal](const string &cmp) {
			if (cmp == "only_once") portal.only_once = true;
		});

		portal.id = row.get<game_portal_id>("id");
		portal.name = row.get<string>("label");
		portal.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		portal.to_map = row.get<game_map_id>("destination");
		portal.to_name = row.get<string>("destination_label");
		portal.script = row.get<string>("script");

		map->add_portal(portal);
	}
}

auto map_data_provider::load_map_life(map *map, game_map_id link) -> void {
	npc_spawn_info npc;
	mob_spawn_info spawn;
	reactor_spawn_info reactor;
	spawn_info life;
	string type;

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_life") << " WHERE mapid = :map",
		soci::use(link, "map"));

	for (const auto &row : rs) {
		life = spawn_info{};
		utilities::str::run_flags(row.get<opt_string>("flags"), [&life](const string &cmp) {
			if (cmp == "faces_left") life.faces_left = true;
		});

		type = row.get<string>("life_type");
		life.id = row.get<int32_t>("lifeid");
		life.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		life.foothold = row.get<game_foothold_id>("foothold");
		life.time = row.get<int32_t>("respawn_time");

		if (type == "npc") {
			npc = npc_spawn_info{};
			npc.set_spawn_info(life);
			npc.rx0 = row.get<game_coord>("min_click_pos");
			npc.rx1 = row.get<game_coord>("max_click_pos");
			map->add_npc(npc);
		}
		else if (type == "mob") {
			spawn = mob_spawn_info{};
			spawn.set_spawn_info(life);
			map->add_mob_spawn(spawn);
		}
		else if (type == "reactor") {
			reactor = reactor_spawn_info{};
			reactor.set_spawn_info(life);
			reactor.name = row.get<string>("life_name");
			map->add_reactor_spawn(reactor);
		}
	}
}

auto map_data_provider::load_footholds(map *map, game_map_id link) -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_footholds") << " WHERE mapid = :map",
		soci::use(link, "map"));

	for (const auto &row : rs) {
		foothold_info foot;
		utilities::str::run_flags(row.get<opt_string>("flags"), [&foot](const string &cmp) {
			if (cmp == "forbid_downward_jump") foot.forbid_jump_down = true;
		});

		foot.id = row.get<game_foothold_id>("id");
		foot.line = line{
			point{row.get<game_coord>("x1"), row.get<game_coord>("y1")},
			point{row.get<game_coord>("x2"), row.get<game_coord>("y2")}
		};
		foot.drag_force = row.get<int16_t>("drag_force");
		foot.left_edge = row.get<game_foothold_id>("previousid") == 0;
		foot.right_edge = row.get<game_foothold_id>("nextid") == 0;
		map->add_foothold(foot);
	}
}

auto map_data_provider::load_map_time_mob(map *map) -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("map_time_mob") << " WHERE mapid = :map", soci::use(map->get_id(), "map"));

	for (const auto &row : rs) {
		ref_ptr<time_mob> info = make_ref_ptr<time_mob>();

		info->id = row.get<game_mob_id>("mobid");
		info->start_hour = row.get<int8_t>("start_hour");
		info->end_hour = row.get<int8_t>("end_hour");
		info->message = row.get<string>("message");
	}
}

auto map_data_provider::get_continent(game_map_id map_id) const -> opt_int8_t {
	int8_t cluster = game_logic_utilities::get_map_cluster(map_id);
	auto kvp = m_continents.find(cluster);
	if (kvp == std::end(m_continents)) {
		return {};
	}
	return kvp->second;
}

}
}