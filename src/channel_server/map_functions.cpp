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
#include "map_functions.hpp"
#include "common/algorithm.hpp"
#include "common/map_position.hpp"
#include "common/util/string.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/party.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include <chrono>
#include <iostream>

namespace vana {
namespace channel_server {

auto map_functions::event_instruction(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->send_map(packets::map::show_event_instructions());
	return chat_result::handled_display;
}

auto map_functions::instruction(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		player->send_map(packets::player::instruction_bubble(args));
		chat_handler_functions::show_info(player, "Showing instruction bubble to everyone on the map");
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto map_functions::timer(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		seconds time{atoi(args.c_str())};
		out_stream msg;
		msg << "Stopped map timer";
		if (time.count() > 0) {
			msg.str("");
			msg.clear();
			msg << "Started map timer. Counting down from ";

			hours hours_value = duration_cast<hours>(time);
			minutes minutes_value = duration_cast<minutes>(time - hours_value);
			seconds seconds_value = duration_cast<seconds>(time - hours_value - minutes_value);

			if (hours_value.count() > 0) {
				msg << hours_value.count() << " hours";
			}
			if (minutes_value.count() > 0) {
				if (hours_value.count() > 0) {
					msg << ", ";
				}
				msg << minutes_value.count() << " minutes";
			}
			if (seconds_value.count() > 0) {
				if (hours_value.count() > 0 || minutes_value.count() > 0) {
					msg << " and ";
				}
				msg << seconds_value.count() << " seconds";
			}
		}
		chat_handler_functions::show_info(player, msg.str());
		player->get_map()->set_map_timer(time);
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto map_functions::kill_mob(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_map_object mob_id = atoi(args.c_str());
		auto mob = player->get_map()->get_mob(mob_id);
		if (mob != nullptr) {
			chat_handler_functions::show_info(player, "Killed mob with map mob ID " + args + ". Damage applied: " + vana::util::str::lexical_cast<string>(mob->get_hp()));
			mob->apply_damage(player->get_id(), mob->get_hp());
		}
		else {
			chat_handler_functions::show_error(player, "Invalid mob: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto map_functions::get_mob_hp(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (!args.empty()) {
		game_map_object mob_id = atoi(args.c_str());
		auto mob = player->get_map()->get_mob(mob_id);
		if (mob != nullptr) {
			out_stream message;
			message << "Mob " << mob_id
					<< " HP: " << mob->get_hp() << "/" << mob->get_max_hp()
					<< " (" << static_cast<int64_t>(mob->get_hp()) * 100 / mob->get_max_hp() << "%)";

			chat_handler_functions::show_info(player, message.str());
		}
		else {
			chat_handler_functions::show_error(player, "Invalid mob: " + args);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto map_functions::list_mobs(ref_ptr<player> player, const game_chat &args) -> chat_result {
	game_map_id map_id = args.empty() ?
		player->get_map_id() :
		chat_handler_functions::get_map(args, player);
	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player, "Invalid map: " + args);
		return chat_result::handled_display;
	}

	chat_handler_functions::show_info(player, "Mobs for Map " + std::to_string(map_id));
	if (map->count_mobs(0) > 0) {
		out_stream message;
		map->run_function_mobs([&message, &player](ref_ptr<const mob> mob) {
			message.str("");
			message.clear();

			message << "Mob " << mob->get_map_mob_id()
				<< " (ID: " << mob->get_mob_id()
				<< ", HP: " << mob->get_hp()
				<< "/" << mob->get_max_hp()
				<< " ~ " << static_cast<int64_t>(mob->get_hp()) * 100 / mob->get_max_hp()
				<< "%) " << mob->get_map_position();

			chat_handler_functions::show_info(player, message.str());
		});
	}
	else {
		chat_handler_functions::show_error(player, "No results");
	}
	return chat_result::handled_display;
}

auto map_functions::list_portals(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	auto result = args.empty() ?
		match_result::no_matches :
		chat_handler_functions::run_regex_pattern(args, R"((\w+)? ?(\w+)?)", matches);

	game_map_id map_id = result == match_result::no_matches ?
		player->get_map_id() :
		chat_handler_functions::get_map(matches[1], player);

	opt_string filter;
	if (result == match_result::any_matches) {
		string match_filter = matches[2];
		if (!match_filter.empty()) {
			filter = match_filter;
		}
	}

	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player, "Invalid map: " + args);
		return chat_result::handled_display;
	}

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT mp.id, mp.label, mp.destination, mp.destination_label, mp.script, mp.x_pos, mp.y_pos "
		<< "FROM " << db.make_table(vana::data::table::map_portals) << " mp "
		<< "WHERE mp.mapid = :map_id",
		soci::use(map_id, "map_id"));

	auto format = [&map](const soci::row &row, out_stream &str) {
		game_map_id destination = row.get<game_map_id>(2);
		opt_string destination_label = row.get<opt_string>(3);
		opt_string portal_script = row.get<opt_string>(4);
		game_coord x = row.get<game_coord>(5);
		game_coord y = row.get<game_coord>(6);

		str << row.get<game_map_id>(0) << " : " << row.get<string>(1);
		if (destination != constant::map::no_map) {
			str << " (destination " << destination;
			if (destination_label.is_initialized()) {
				string label = destination_label.get();
				if (!label.empty()) {
					str << " -> " << destination_label.get();
				}
			}
			str << ")";
		}

		if (portal_script.is_initialized()) {
			string script = portal_script.get();
			if (!script.empty()) {
				str << " (script '" << script << "')";
			}
		}

		auto pos = point{x, y};
		auto search_rect = rect{x - 25, y - 20, 50, 70};
		point floor_pos;
		game_foothold_id foothold = 0;
		if (map->find_floor(pos, floor_pos, -20, search_rect) == search_result::not_found) {
			foothold = map->get_foothold_at_position(pos);
		}
		else {
			foothold = map->get_foothold_at_position(floor_pos);
		}
		str << " " << map_position{pos, foothold};
	};

	if (filter.is_initialized()) {
		chat_handler_functions::show_info(player, "Portals with label '" + filter.get() + "' for Map " + std::to_string(map_id));
	}
	else {
		chat_handler_functions::show_info(player, "Portals for Map " + std::to_string(map_id));
	}
	out_stream str{""};
	bool found = false;
	for (const auto &row : rs) {
		string portal_label = row.get<string>(1);
		if (filter.is_initialized()) {
			if (vana::util::str::no_case_compare(filter.get(), portal_label) != 0) {
				continue;
			}
		}
		else if (portal_label == "sp" || portal_label == "tp") {
			continue;
		}
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		chat_handler_functions::show_info(player, str.str());
	}

	if (!found) {
		chat_handler_functions::show_error(player, "No results");
	}

	return chat_result::handled_display;
}

auto map_functions::list_players(ref_ptr<player> player_value, const game_chat &args) -> chat_result {
	game_map_id map_id = args.empty() ?
		player_value->get_map_id() :
		chat_handler_functions::get_map(args, player_value);
	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player_value, "invalid map: " + args);
		return chat_result::handled_display;
	}

	chat_handler_functions::show_info(player_value, "Players for Map " + std::to_string(map_id));

	out_stream str{""};
	bool found = false;
	map->run_function_players([&](ref_ptr<player> target) {
		if (target->is_using_gm_hide()) {
			return;
		}

		found = true;
		str.str("");
		str.clear();

		str << vana::util::str::to_upper(target->get_name())
			<< " (ID: " << target->get_id() << ", ";

		if (party *party = target->get_party()) {
			str << "Party ID: " << party->get_id() << ", ";
		}
			
		str << "HP: " << target->get_stats()->get_hp() << "/" << target->get_stats()->get_max_hp() << " ~ "
			<< target->get_stats()->get_hp() * 100 / target->get_stats()->get_max_hp() << "%) "
			<< target->get_map_position();

		chat_handler_functions::show_info(player_value, str.str());
	});

	if (!found) {
		chat_handler_functions::show_error(player_value, "No results");
	}

	return chat_result::handled_display;
}

auto map_functions::list_reactors(ref_ptr<player> player, const game_chat &args) -> chat_result {
	game_map_id map_id = args.empty() ?
		player->get_map_id() :
		chat_handler_functions::get_map(args, player);
	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player, "Invalid map: " + args);
		return chat_result::handled_display;
	}

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT ml.lifeid, sc.script, ml.x_pos, ml.y_pos, ml.foothold "
		<< "FROM " << db.make_table(vana::data::table::map_life) << " ml "
		<< "LEFT OUTER JOIN " << db.make_table(vana::data::table::scripts) << " sc ON sc.objectid = ml.lifeid AND sc.script_type = 'reactor' "
		<< "WHERE ml.life_type = 'reactor' AND ml.mapid = :mapId",
		soci::use(map_id, "map_id"));

	auto format = [](const soci::row &row, out_stream &str) {
		opt_string reactor_script = row.get<opt_string>(1);
		game_coord x = row.get<game_coord>(2);
		game_coord y = row.get<game_coord>(3);
		game_foothold_id foothold = row.get<game_foothold_id>(4);

		str << row.get<game_reactor_id>(0);
		if (reactor_script.is_initialized()) {
			string script = reactor_script.get();
			if (!script.empty()) {
				str << " (script '" << script << "')";
			}
		}

		str << " " << map_position{point{x, y}, foothold};
	};

	chat_handler_functions::show_info(player, "Reactors for Map " + std::to_string(map_id));

	out_stream str{""};
	bool found = false;
	for (const auto &row : rs) {
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		chat_handler_functions::show_info(player, str.str());
	}

	if (!found) {
		chat_handler_functions::show_error(player, "No results");
	}

	return chat_result::handled_display;
}

auto map_functions::list_npcs(ref_ptr<player> player, const game_chat &args) -> chat_result {
	game_map_id map_id = args.empty() ?
		player->get_map_id() :
		chat_handler_functions::get_map(args, player);
	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player, "Invalid map: " + args);
		return chat_result::handled_display;
	}

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT ml.lifeid, st.label, sc.script, ml.x_pos, ml.y_pos, ml.foothold "
		<< "FROM " << db.make_table(vana::data::table::map_life) << " ml "
		<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " st ON st.objectid = ml.lifeid AND st.object_type = 'npc' "
		<< "LEFT OUTER JOIN " << db.make_table(vana::data::table::scripts) << " sc ON sc.objectid = ml.lifeid AND sc.script_type = 'npc' "
		<< "WHERE ml.life_type = 'npc' AND ml.mapid = :mapId",
		soci::use(map_id, "map_id"));

	auto format = [](const soci::row &row, out_stream &str) {
		str << row.get<game_npc_id>(0) << " : " << row.get<string>(1);
		opt_string script = row.get<opt_string>(2);
		game_coord x = row.get<game_coord>(3);
		game_coord y = row.get<game_coord>(4);
		game_foothold_id foothold = row.get<game_foothold_id>(5);

		if (script.is_initialized()) {
			str << " (script '" << script.get() << "')";
		}

		str << " " << map_position{point{x, y}, foothold};
	};

	chat_handler_functions::show_info(player, "NPCs for Map " + std::to_string(map_id));

	out_stream str{""};
	bool found = false;
	for (const auto &row : rs) {
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		chat_handler_functions::show_info(player, str.str());
	}

	if (!found) {
		chat_handler_functions::show_error(player, "No results");
	}

	return chat_result::handled_display;
}

auto map_functions::map_dimensions(ref_ptr<player> player, const game_chat &args) -> chat_result {
	game_map_id map_id = args.empty() ?
		player->get_map_id() :
		chat_handler_functions::get_map(args, player);
	map *map = maps::get_map(map_id);
	if (map == nullptr) {
		chat_handler_functions::show_error(player, "Invalid map: " + args);
	}
	else {
		chat_handler_functions::show_info(player, [&](out_stream &message) {
			message << "Dimensions for Map " << map_id << ": " << map->get_dimensions();
		});
	}
	return chat_result::handled_display;
}

auto map_functions::zakum(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->get_map()->spawn_zakum(player->get_pos());
	channel_server::get_instance().log(vana::log::type::gm_command, [&](out_stream &log) {
		log << "GM " << player->get_name()
			<< " spawned Zakum on map " << player->get_map_id();
	});
	return chat_result::handled_display;
}

auto map_functions::horntail(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->get_map()->spawn_mob(constant::mob::summon_horntail, player->get_pos());
	channel_server::get_instance().log(vana::log::type::gm_command, [&](out_stream &log) {
		log << "GM " << player->get_name()
			<< " spawned Horntail on map " << player->get_map_id();
	});
	return chat_result::handled_display;
}

auto map_functions::music(ref_ptr<player> player, const game_chat &args) -> chat_result {
	if (args.empty()) {
		chat_handler_functions::show_info(player, "Current music: " + player->get_map()->get_music());
	}
	else {
		auto &db = vana::io::database::get_data_db();
		auto &sql = db.get_session();
		string music;

		if (args == "default") {
			music = args;
		}
		else {
			sql
				<< "SELECT m.default_bgm "
				<< "FROM " << db.make_table(vana::data::table::map_data) << " m "
				<< "WHERE m.default_bgm = :q "
				<< "LIMIT 1",
				soci::use(args, "q"),
				soci::into(music);
		}

		if (music.empty()) {
			chat_handler_functions::show_error(player, "Invalid music: " + args);
		}
		else {
			player->get_map()->set_music(music);
			chat_handler_functions::show_info(player, "Set music on the map to: " + music);
		}
	}
	return chat_result::handled_display;
}

auto map_functions::summon(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) ?(\d+)?)", matches) == match_result::any_matches) {
		string raw_mob_id = matches[1];
		game_mob_id mob_id = atoi(raw_mob_id.c_str());
		if (channel_server::get_instance().get_mob_data_provider().mob_exists(mob_id)) {
			string countString = matches[2];
			int32_t count = ext::constrain_range(countString.empty() ? 1 : atoi(countString.c_str()), 1, 1000);
			for (int32_t i = 0; i < count; ++i) {
				player->get_map()->spawn_mob(mob_id, player->get_pos());
			}
			if (count > 0) {
				chat_handler_functions::show_info(player, [&](out_stream &message) {
					message << "Spawned " << count
						<< " mobs with ID " << mob_id;
				});
			}
			else {
				chat_handler_functions::show_error(player, "No mobs spawned");
			}
		}
		else {
			chat_handler_functions::show_error(player, "Invalid mob: " + raw_mob_id);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto map_functions::clear_drops(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->get_map()->clear_drops();
	return chat_result::handled_display;
}

auto map_functions::kill_all_mobs(ref_ptr<player> player, const game_chat &args) -> chat_result {
	int32_t killed = player->get_map()->kill_mobs(player, true);
	chat_handler_functions::show_info(player, [&](out_stream &message) {
		message << "Killed " << killed << " mobs";
	});
	return chat_result::handled_display;
}

}
}