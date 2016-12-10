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
#include "map.hpp"
#include "common/algorithm.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/map.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <utility>

namespace vana {
namespace data {
namespace provider {

auto map::load_data() -> void {
	load_continents();
	load_maps();
}

auto map::load_continents() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Continents... ";

	decltype(m_continents) copy;
	int8_t map_cluster;
	int8_t continent;

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_continent_data));

	for (const auto &row : rs) {
		map_cluster = row.get<int8_t>("map_cluster");
		continent = row.get<int8_t>("continent");

		copy.push_back(std::make_pair(map_cluster, continent));
	}

	{
		owned_lock<mutex> l{m_load_mutex};
		m_continents = copy;
	}

	std::cout << "DONE" << std::endl;
}

auto map::load_maps() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Maps... ";

	decltype(m_maps) copy;

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_data));

	for (const auto &row : rs) {
		auto info = make_ref_ptr<data::type::map_info>();
		game_map_id id = row.get<game_map_id>("mapid");
		game_map_id link = row.get<game_map_id>("link");

		info->id = id;
		info->link = link;

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
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

		vana::util::str::run_flags(row.get<opt_string>("field_type"), [&info](const string &cmp) {
			if (cmp == "force_map_equip") info->force_map_equip = true;
		});

		vana::util::str::run_flags(row.get<opt_string>("field_limitations"), [&info](const string &cmp) mutable {
			if (cmp == "jump") info->limitations.jump = true;
			else if (cmp == "movement_skills") info->limitations.movement_skills = true;
			else if (cmp == "summoning_bag") info->limitations.summoning_bag = true;
			else if (cmp == "mystic_door") info->limitations.mystic_door = true;
			else if (cmp == "channel_switching") info->limitations.channel_switching = true;
			else if (cmp == "regular_exp_loss") info->limitations.regular_exp_loss = true;
			else if (cmp == "vip_rock") info->limitations.vip_rock = true;
			else if (cmp == "minigames") info->limitations.minigames = true;
			else if (cmp == "mount") info->limitations.mount = true;
			else if (cmp == "potion_use") info->limitations.potion_use = true;
			else if (cmp == "drop_down") info->limitations.drop_down = true;
			else if (cmp == "chalkboard") info->limitations.chalkboard = true;
		});

		// There are no cases as of .75 where:
		// link != 0 && FLOOR(link / 10000000) != FLOOR(mapid / 10000000)
		// Therefore I don't know if it's the link that's important or the map ID
		// I assume it's the map ID
		info->continent = get_continent(id).get(0);
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

		copy.push_back(info);
	}

	/*
	// This adds about 40 seconds to the debug startup process (the process is roughly 20 seconds currently)
	// This isn't desirable for now
	for (auto &map : m_maps) {
		load_map(map);
	}
	*/

	{
		owned_lock<mutex> l{m_load_mutex};
		m_maps = copy;
		m_link_info.clear();
	}

	std::cout << "DONE" << std::endl;
}

auto map::load_map(data::type::map_info &map) -> void {
	size_t index = 0;
	bool found = false;
	for (/* intentionally blank */; index < m_link_info.size(); ++index) {
		auto &value = m_link_info[index];
		found =
			(map.link != 0 && value->id == map.link) ||
			(map.link == 0 && value->id == map.id);

		if (found) {
			break;
		}
	}

	if (!found) {
		auto info = make_ref_ptr<data::type::map_link_info>();
		info->id = map.link != 0 ? map.link : map.id;

		load_map_time_mob(*info);
		load_footholds(*info);
		load_map_life(*info);
		load_portals(*info);
		load_seats(*info);

		m_link_info.push_back(info);

		map.link_info = m_link_info[m_link_info.size() - 1];
	}
	else {
		map.link_info = m_link_info[index];
	}
}

auto map::load_seats(data::type::map_link_info &map) -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_seats) << " WHERE mapid = :map",
		soci::use(map.id, "map"));

	for (const auto &row : rs) {
		data::type::seat_info chair;
		game_seat_id id = row.get<game_seat_id>("seatid");
		chair.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		chair.id = id;

		map.seats.push_back(chair);
	}
}

auto map::load_portals(data::type::map_link_info &map) -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_portals) << " WHERE mapid = :map",
		soci::use(map.id, "map"));

	for (const auto &row : rs) {
		data::type::portal_info portal;
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&portal](const string &cmp) {
			if (cmp == "only_once") portal.only_once = true;
		});

		portal.id = row.get<game_portal_id>("id");
		portal.name = row.get<string>("label");
		portal.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		portal.to_map = row.get<game_map_id>("destination");
		portal.to_name = row.get<string>("destination_label");
		portal.script = row.get<string>("script");

		map.portals.push_back(portal);
	}
}

auto map::load_map_life(data::type::map_link_info &map) -> void {
	data::type::npc_spawn_info npc;
	data::type::mob_spawn_info spawn;
	data::type::reactor_spawn_info reactor;
	data::type::spawn_info life;
	string type;

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_life) << " WHERE mapid = :map",
		soci::use(map.id, "map"));

	for (const auto &row : rs) {
		life = data::type::spawn_info{};
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&life](const string &cmp) {
			if (cmp == "faces_left") life.faces_left = true;
		});

		type = row.get<string>("life_type");
		life.id = row.get<int32_t>("lifeid");
		life.pos = point{row.get<game_coord>("x_pos"), row.get<game_coord>("y_pos")};
		life.foothold = row.get<game_foothold_id>("foothold");
		life.time = row.get<int32_t>("respawn_time");

		if (type == "npc") {
			npc = data::type::npc_spawn_info{};
			npc.set_spawn_info(life);
			npc.rx0 = row.get<game_coord>("min_click_pos");
			npc.rx1 = row.get<game_coord>("max_click_pos");
			map.npcs.push_back(npc);
		}
		else if (type == "mob") {
			spawn = data::type::mob_spawn_info{};
			spawn.set_spawn_info(life);
			map.mobs.push_back(spawn);
		}
		else if (type == "reactor") {
			reactor = data::type::reactor_spawn_info{};
			reactor.set_spawn_info(life);
			reactor.name = row.get<string>("life_name");
			map.reactors.push_back(reactor);
		}
		else THROW_CODE_EXCEPTION(not_implemented_exception);
	}
}

auto map::load_footholds(data::type::map_link_info &map) -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_footholds) << " WHERE mapid = :map",
		soci::use(map.id, "map"));

	for (const auto &row : rs) {
		data::type::foothold_info foot;
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&foot](const string &cmp) {
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

		map.footholds.push_back(foot);
	}
}

auto map::load_map_time_mob(data::type::map_link_info &map) -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::map_time_mob) << " WHERE mapid = :map",
		soci::use(map.id, "map"));

	for (const auto &row : rs) {
		data::type::time_mob_info info{};

		info.id = row.get<game_mob_id>("mobid");
		info.start_hour = row.get<int8_t>("start_hour");
		info.end_hour = row.get<int8_t>("end_hour");
		info.message = row.get<string>("message");

		map.time_mob = info;
	}
}

auto map::get_continent(game_map_id map_id) -> opt_int8_t {
	int8_t cluster = vana::util::game_logic::map::get_map_cluster(map_id);

	owned_lock<mutex> l{m_load_mutex};
	for (const auto &continent : m_continents) {
		if (continent.first == cluster) {
			return continent.second;
		}
	}

	return {};
}

auto map::get_map(game_map_id map_id) -> ref_ptr<const data::type::map_info> {
	ref_ptr<data::type::map_info> ptr;
	owned_lock<mutex> l{m_load_mutex};
	for (auto &map : m_maps) {
		if (map->id == map_id) {
			ptr = map;
			break;
		}
	}

	if (ptr == nullptr) THROW_CODE_EXCEPTION(codepath_invalid_exception);

	if (ptr->link_info == nullptr) {
		load_map(*ptr);
	}

	return ptr;
}

}
}
}