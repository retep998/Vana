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
#include "info_functions.hpp"
#include "common/io/database.hpp"
#include "common/map_position.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include <iostream>

namespace vana {
namespace channel_server {

auto info_functions::help(ref_ptr<player> player, const game_chat &args) -> chat_result {
	using chat_handler_functions::g_command_list;
	if (!args.empty()) {
		if (g_command_list.find(args) != std::end(g_command_list)) {
			chat_handler_functions::show_syntax(player, args, true);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid command: " + args);
		}
	}
	else {
		bool has = false;
		out_stream strm;
		strm << "You may not use any commands.";
		for (const auto &kvp : g_command_list) {
			if (player->get_gm_level() >= kvp.second.level) {
				if (!has) {
					strm.str("");
					strm.clear();
					strm << "Available commands: ";
					has = true;
				}
				strm << kvp.first << " ";
			}
		}
		chat_handler_functions::show_info(player, strm.str());
	}
	return chat_result::handled_display;
}

auto info_functions::lookup(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) ?(.+)?)", matches) == match_result::any_matches) {
		uint16_t type = 0;
		uint16_t sub_type = 0;
		string raw_type = matches[1];
		uint16_t non_mcdb_type = 999;

		// These constants correspond to MCDB enum types
		if (raw_type == "item") type = 1;
		else if (raw_type == "equip") { type = 1; sub_type = 1; }
		else if (raw_type == "use") { type = 1; sub_type = 2; }
		else if (raw_type == "setup") { type = 1; sub_type = 3; }
		else if (raw_type == "etc") { type = 1; sub_type = 4; }
		else if (raw_type == "cash") { type = 1; sub_type = 5; }
		else if (raw_type == "skill") type = 2;
		else if (raw_type == "map") type = 3;
		else if (raw_type == "mob") type = 4;
		else if (raw_type == "npc") type = 5;
		else if (raw_type == "quest") type = 6;
		else type = non_mcdb_type;

		auto is_integer_string = [](const string &input) -> bool {
			return std::all_of(std::cbegin(input), std::cend(input), [](char c) -> bool {
				return c >= '0' && c <= '9';
			});
		};
		auto should_be_id_only = [player](const string &type, const string &input) -> chat_result {
			chat_handler_functions::show_error(player, type + " should be given an integral identifier. Input was: " + input);
			return chat_result::handled_display;
		};
		auto requires_second_argument = [player](const string &type) -> chat_result {
			chat_handler_functions::show_error(player, type + " requires a second argument");
			return chat_result::handled_display;
		};

		auto &db = vana::io::database::get_data_db();
		auto &sql = db.get_session();
		auto display_func = [&sql, &player](const soci::rowset<> &rs, function<void(const soci::row &row, out_stream &str)> format_message, const string &query) {
			// Bug in the behavior of SOCI
			// In the case where you use dynamic resultset binding, got_data() will not properly report that it got results

			chat_handler_functions::show_info(player, "Search for '" + query + "'");

			out_stream str{""};
			bool found = false;
			for (const auto &row : rs) {
				found = true;

				str.str("");
				str.clear();
				format_message(row, str);
				chat_handler_functions::show_info(player, str.str());
			}

			if (!found) {
				chat_handler_functions::show_error(player, "No results");
			}
		};

		if (type < non_mcdb_type) {
			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string>(1);
			};

			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}

			if (type == 1 && sub_type != 0) {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT s.objectid, s.`label` "
					<< "FROM " << db.make_table(vana::data::table::strings) << " s "
					<< "INNER JOIN " << db.make_table(vana::data::table::item_data) << " i ON s.objectid = i.itemid "
					<< "WHERE s.object_type = :type AND s.label LIKE :q AND i.inventory = :subtype",
					soci::use(q, "q"),
					soci::use(type, "type"),
					soci::use(sub_type, "subtype"));

				display_func(rs, format, matches[2]);
			}
			else {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT objectid, `label` "
					<< "FROM " << db.make_table(vana::data::table::strings) << " "
					<< "WHERE object_type = :type AND label LIKE :q",
					soci::use(q, "q"),
					soci::use(type, "type"));

				display_func(rs, format, matches[2]);
			}
		}
		else if (raw_type == "id") {
			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			if (!is_integer_string(q)) {
				return should_be_id_only("id", q);
			}

			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<int32_t>(0) << " (" << row.get<string>(2) << ") : " << row.get<string>(1);
			};

			soci::rowset<> rs = (sql.prepare << "SELECT objectid, `label`, object_type FROM " << db.make_table(vana::data::table::strings) << " WHERE objectid = :q", soci::use(q, "q"));
			display_func(rs, format, matches[2]);
		}
		else if (raw_type == "continent") {
			string raw_map = matches[2];
			game_map_id map_id = raw_map.empty() ?
				player->get_map_id() :
				chat_handler_functions::get_map(raw_map, player);

			if (maps::get_map(map_id) != nullptr) {
				chat_handler_functions::show_info(player, [&](out_stream &message) {
					auto cont = channel_server::get_instance().get_map_data_provider().get_continent(map_id);
					if (!cont.is_initialized()) {
						message << map_id << " does not have a continent ID";
					}
					else {
						message << "Continent ID of " << map_id << " : " << static_cast<int32_t>(cont.get());
					}
				});
			}
			else {
				chat_handler_functions::show_error(player, "Invalid map: " + raw_map);
			}
		}
		else if (raw_type == "scriptbyname" || raw_type == "scriptbyid") {
			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<int32_t>(1) << " (" << row.get<string>(0) << ") : " << row.get<string>(2);
			};

			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			if (raw_type == "scriptbyname") {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << db.make_table(vana::data::table::scripts) << " WHERE script LIKE :q",
					soci::use(q, "q"));
				display_func(rs, format, matches[2]);
			}
			else if (raw_type == "scriptbyid") {
				if (!is_integer_string(q)) {
					return should_be_id_only("scriptbyid", q);
				}
				soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << db.make_table(vana::data::table::scripts) << " WHERE objectid = :q",
					soci::use(q, "q"));
				display_func(rs, format, matches[2]);
			}
		}
		else if (raw_type == "whatdrops") {
			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string>(1);
			};

			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			if (!is_integer_string(q)) {
				return should_be_id_only("whatdrops", q);
			}

			soci::rowset<> rs = (sql.prepare
				<< "SELECT d.dropperid, s.label "
				<< "FROM " << db.make_table(vana::data::table::drop_data) << " d "
				<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
				<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << db.make_table(vana::data::table::user_drop_data) << ") AND d.itemid = :q "
				<< "UNION ALL "
				<< "SELECT d.dropperid, s.label "
				<< "FROM " << db.make_table(vana::data::table::user_drop_data) << " d "
				<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
				<< "WHERE d.itemid = :q ",
				soci::use(q, "q"));

			display_func(rs, format, matches[2]);
		}
		else if (raw_type == "whatmaps") {
			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			if (chat_handler_functions::run_regex_pattern(q, R"((\w+) (\w+))", matches) == match_result::any_matches) {
				auto format = [](const soci::row &row, out_stream &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string>(1);
				};

				string option = matches[1];
				string test = matches[2];
				if (option == "portal") {
					soci::rowset<> rs = (sql.prepare
						<< "SELECT m.mapid, s.label "
						<< "FROM " << db.make_table(vana::data::table::map_data) << " m "
						<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = m.mapid AND s.object_type = 'map' "
						<< "WHERE m.mapid IN ("
						<< "	SELECT mp.mapid "
						<< "	FROM " << db.make_table(vana::data::table::map_portals) << " mp "
						<< "	WHERE mp.script = :query "
						<< ")",
						soci::use(test, "query"));

					display_func(rs, format, matches[2]);
				}
				else if (option == "npc" || option == "mob" || option == "reactor") {
					if (!is_integer_string(test)) {
						return should_be_id_only("whatmaps+" + option, test);
					}

					option = " AND ml.life_type = '" + option + "'";

					soci::rowset<> rs = (sql.prepare
						<< "SELECT m.mapid, s.label "
						<< "FROM " << db.make_table(vana::data::table::map_data) << " m "
						<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = m.mapid AND s.object_type = 'map' "
						<< "WHERE m.mapid IN ("
						<< "	SELECT ml.mapid "
						<< "	FROM " << db.make_table(vana::data::table::map_life) << " ml "
						<< "	WHERE ml.lifeid = :object_id "
						<< "	" << option
						<< ")",
						soci::use(test, "object_id"));

					display_func(rs, format, matches[2]);
				}
				else {
					chat_handler_functions::show_error(player, "Invalid life type: " + option);
					return chat_result::handled_display;
				}
			}
			else {
				chat_handler_functions::show_error(player, "whatmaps should be given a type indicator (valid ones are npc, mob, portal, and reactor) and an argument to match against. Input was: " + q);
				return chat_result::handled_display;
			}
		}
		else if (raw_type == "music") {
			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<string>(0);
			};

			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			q = "%" + q + "%";
			soci::rowset<> rs = (sql.prepare
				<< "SELECT DISTINCT m.default_bgm "
				<< "FROM " << db.make_table(vana::data::table::map_data) << " m "
				<< "WHERE m.default_bgm LIKE :q",
				soci::use(q, "q"));

			display_func(rs, format, matches[2]);
		}
		else if (raw_type == "drops") {
			auto format = [](const soci::row &row, out_stream &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string>(1) << " (base rate " << (static_cast<double>(row.get<int32_t>(2)) / 1000000. * 100.) << "%)";
			};

			string q = matches[2];
			if (q.empty()) {
				return requires_second_argument(raw_type);
			}
			if (!is_integer_string(q)) {
				return should_be_id_only("drops", q);
			}

			soci::rowset<> rs = (sql.prepare
				<< "SELECT d.itemid, s.label, d.chance "
				<< "FROM " << db.make_table(vana::data::table::drop_data) << " d "
				<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
				<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << db.make_table(vana::data::table::user_drop_data) << ") AND d.dropperid = :q "
				<< "UNION ALL "
				<< "SELECT d.itemid, s.label, d.chance "
				<< "FROM " << db.make_table(vana::data::table::user_drop_data) << " d "
				<< "INNER JOIN " << db.make_table(vana::data::table::strings) << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
				<< "WHERE d.dropperid = :q "
				<< "ORDER BY itemid",
				soci::use(q, "q"));

			display_func(rs, format, matches[2]);
		}
		else {
			chat_handler_functions::show_error(player, "Invalid search type: " + raw_type);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto info_functions::pos(ref_ptr<player> player, const game_chat &args) -> chat_result {
	chat_handler_functions::show_info(player, [&](out_stream &message) { message << player->get_map_position(); });
	return chat_result::handled_display;
}

auto info_functions::online(ref_ptr<player> player_value, const game_chat &args) -> chat_result {
	out_stream igns;
	igns << "IGNs: ";
	int32_t i = 0;
	const int32_t max = 100;
	channel_server::get_instance().get_player_data_provider().run([&i, &max, &igns](ref_ptr<player> player) {
		if (i < max) {
			if (i != 0) {
				igns << ", ";
			}
			igns << player->get_name();
			i++;
		}
	});
	chat_handler_functions::show_info(player_value, igns.str());
	return chat_result::handled_display;
}

auto info_functions::variable(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+))", matches) == match_result::no_matches) {
		return chat_result::show_syntax;
	}

	string key = matches[1];
	string val = player->get_variables()->get_variable(key);
	if (!val.empty()) {
		chat_handler_functions::show_info(player, key + ": " + val);
	}
	else {
		chat_handler_functions::show_error(player, "Invalid variable: " + key);
	}
	return chat_result::handled_display;
}

auto info_functions::quest_data(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) (\w+))", matches) == match_result::no_matches) {
		return chat_result::show_syntax;
	}

	string quest = matches[1];
	string data = matches[2];
	game_quest_id id = atoi(quest.c_str());
	player->get_quests()->set_quest_data(id, data);
	return chat_result::handled_display;
}

auto info_functions::quest_kills(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\d+) (\d+))", matches) == match_result::no_matches) {
		return chat_result::show_syntax;
	}

	string mob = matches[1];
	string kills = matches[2];

	game_mob_id mob_id = atoi(mob.c_str());
	int32_t count = atoi(kills.c_str());

	for (int32_t i = 0; i < count; i++) {
		player->get_quests()->update_quest_mob(mob_id);
	}

	return chat_result::handled_display;
}

auto info_functions::gm_level(ref_ptr<player> player, const game_chat &args) -> chat_result {
	chat_handler_functions::show_info(player, [&](game_chat_stream &message) { message << "Your GM level: " << player->get_gm_level(); });
	return chat_result::handled_display;
}

}
}