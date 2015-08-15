/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "InfoFunctions.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include <iostream>

auto InfoFunctions::help(Player *player, const chat_t &args) -> ChatResult {
	using ChatHandlerFunctions::sCommandList;
	if (!args.empty()) {
		if (sCommandList.find(args) != std::end(sCommandList)) {
			ChatHandlerFunctions::showSyntax(player, args, true);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid command: " + args);
		}
	}
	else {
		bool has = false;
		out_stream_t strm;
		strm << "You may not use any commands.";
		for (const auto &kvp : sCommandList) {
			if (player->getGmLevel() >= kvp.second.level) {
				if (!has) {
					strm.str("");
					strm.clear();
					strm << "Available commands: ";
					has = true;
				}
				strm << kvp.first << " ";
			}
		}
		ChatHandlerFunctions::showInfo(player, strm.str());
	}
	return ChatResult::HandledDisplay;
}

auto InfoFunctions::lookup(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) ?(.+)?)", matches) == MatchResult::AnyMatches) {
		uint16_t type = 0;
		uint16_t subType = 0;
		string_t rawType = matches[1];
		uint16_t nonMcdbType = 999;

		// These constants correspond to MCDB enum types
		if (rawType == "item") type = 1;
		else if (rawType == "equip") { type = 1; subType = 1; }
		else if (rawType == "use") { type = 1; subType = 2; }
		else if (rawType == "setup") { type = 1; subType = 3; }
		else if (rawType == "etc") { type = 1; subType = 4; }
		else if (rawType == "cash") { type = 1; subType = 5; }
		else if (rawType == "skill") type = 2;
		else if (rawType == "map") type = 3;
		else if (rawType == "mob") type = 4;
		else if (rawType == "npc") type = 5;
		else if (rawType == "quest") type = 6;
		else type = nonMcdbType;

		auto isIntegerString = [](const string_t &input) -> bool {
			return std::all_of(std::cbegin(input), std::cend(input), [](char c) -> bool {
				return c >= '0' && c <= '9';
			});
		};
		auto shouldBeIdOnly = [player](const string_t &type, const string_t &input) -> ChatResult {
			ChatHandlerFunctions::showError(player, type + " should be given an integral identifier. Input was: " + input);
			return ChatResult::HandledDisplay;
		};
		auto requiresSecondArgument = [player](const string_t &type) -> ChatResult {
			ChatHandlerFunctions::showError(player, type + " requires a second argument");
			return ChatResult::HandledDisplay;
		};

		auto &db = Database::getDataDb();
		auto &sql = db.getSession();
		auto displayFunc = [&sql, &player](const soci::rowset<> &rs, function_t<void(const soci::row &row, out_stream_t &str)> formatMessage, const string_t &query) {
			// Bug in the behavior of SOCI
			// In the case where you use dynamic resultset binding, got_data() will not properly report that it got results

			ChatHandlerFunctions::showInfo(player, "Search for '" + query + "'");

			out_stream_t str("");
			bool found = false;
			for (const auto &row : rs) {
				found = true;

				str.str("");
				str.clear();
				formatMessage(row, str);
				ChatHandlerFunctions::showInfo(player, str.str());
			}

			if (!found) {
				ChatHandlerFunctions::showError(player, "No results");
			}
		};

		if (type < nonMcdbType) {
			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
			};

			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}

			if (type == 1 && subType != 0) {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT s.objectid, s.`label` "
					<< "FROM " << db.makeTable("strings") << " s "
					<< "INNER JOIN " << db.makeTable("item_data") << " i ON s.objectid = i.itemid "
					<< "WHERE s.object_type = :type AND s.label LIKE :q AND i.inventory = :subtype",
					soci::use(q, "q"),
					soci::use(type, "type"),
					soci::use(subType, "subtype"));

				displayFunc(rs, format, matches[2]);
			}
			else {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT objectid, `label` "
					<< "FROM " << db.makeTable("strings") << " "
					<< "WHERE object_type = :type AND label LIKE :q",
					soci::use(q, "q"),
					soci::use(type, "type"));

				displayFunc(rs, format, matches[2]);
			}
		}
		else if (rawType == "id") {
			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			if (!isIntegerString(q)) {
				return shouldBeIdOnly("id", q);
			}

			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<int32_t>(0) << " (" << row.get<string_t>(2) << ") : " << row.get<string_t>(1);
			};

			soci::rowset<> rs = (sql.prepare << "SELECT objectid, `label`, object_type FROM " << db.makeTable("strings") << " WHERE objectid = :q", soci::use(q, "q"));
			displayFunc(rs, format, matches[2]);
		}
		else if (rawType == "continent") {
			string_t rawMap = matches[2];
			map_id_t mapId = rawMap.empty() ?
				player->getMapId() :
				ChatHandlerFunctions::getMap(rawMap, player);

			if (Maps::getMap(mapId) != nullptr) {
				ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) {
					auto cont = ChannelServer::getInstance().getMapDataProvider().getContinent(mapId);
					if (!cont.is_initialized()) {
						message << mapId << " does not have a continent ID";
					}
					else {
						message << "Continent ID of " << mapId << " : " << static_cast<int32_t>(cont.get());
					}
				});
			}
			else {
				ChatHandlerFunctions::showError(player, "Invalid map: " + rawMap);
			}
		}
		else if (rawType == "scriptbyname" || rawType == "scriptbyid") {
			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<int32_t>(1) << " (" << row.get<string_t>(0) << ") : " << row.get<string_t>(2);
			};

			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			if (rawType == "scriptbyname") {
				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << db.makeTable("scripts") << " WHERE script LIKE :q", soci::use(q, "q"));
				displayFunc(rs, format, matches[2]);
			}
			else if (rawType == "scriptbyid") {
				if (!isIntegerString(q)) {
					return shouldBeIdOnly("scriptbyid", q);
				}
				soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << db.makeTable("scripts") << " WHERE objectid = :q", soci::use(q, "q"));
				displayFunc(rs, format, matches[2]);
			}
		}
		else if (rawType == "whatdrops") {
			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
			};

			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			if (!isIntegerString(q)) {
				return shouldBeIdOnly("whatdrops", q);
			}

			soci::rowset<> rs = (sql.prepare
				<< "SELECT d.dropperid, s.label "
				<< "FROM " << db.makeTable("drop_data") << " d "
				<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
				<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << db.makeTable("user_drop_data") << ") AND d.itemid = :q "
				<< "UNION ALL "
				<< "SELECT d.dropperid, s.label "
				<< "FROM " << db.makeTable("user_drop_data") << " d "
				<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
				<< "WHERE d.itemid = :q ",
				soci::use(q, "q"));

			displayFunc(rs, format, matches[2]);
		}
		else if (rawType == "whatmaps") {
			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			if (ChatHandlerFunctions::runRegexPattern(q, R"((\w+) (\w+))", matches) == MatchResult::AnyMatches) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
				};

				string_t option = matches[1];
				string_t test = matches[2];
				if (option == "portal") {
					soci::rowset<> rs = (sql.prepare
						<< "SELECT m.mapid, s.label "
						<< "FROM " << db.makeTable("map_data") << " m "
						<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = m.mapid AND s.object_type = 'map' "
						<< "WHERE m.mapid IN ("
						<< "	SELECT mp.mapid "
						<< "	FROM " << db.makeTable("map_portals") << " mp "
						<< "	WHERE mp.script = :query "
						<< ")",
						soci::use(test, "query"));

					displayFunc(rs, format, matches[2]);
				}
				else if (option == "npc" || option == "mob" || option == "reactor") {
					if (!isIntegerString(test)) {
						return shouldBeIdOnly("whatmaps+" + option, test);
					}

					option = " AND ml.life_type = '" + option + "'";

					soci::rowset<> rs = (sql.prepare
						<< "SELECT m.mapid, s.label "
						<< "FROM " << db.makeTable("map_data") << " m "
						<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = m.mapid AND s.object_type = 'map' "
						<< "WHERE m.mapid IN ("
						<< "	SELECT ml.mapid "
						<< "	FROM " << db.makeTable("map_life") << " ml "
						<< "	WHERE ml.lifeid = :objectId "
						<< "	" << option
						<< ")",
						soci::use(test, "objectId"));

					displayFunc(rs, format, matches[2]);
				}
				else {
					ChatHandlerFunctions::showError(player, "Invalid life type: " + option);
					return ChatResult::HandledDisplay;
				}
			}
			else {
				ChatHandlerFunctions::showError(player, "whatmaps should be given a type indicator (valid ones are npc, mob, portal, and reactor) and an argument to match against. Input was: " + q);
				return ChatResult::HandledDisplay;
			}
		}
		else if (rawType == "music") {
			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<string_t>(0);
			};

			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			q = "%" + q + "%";
			soci::rowset<> rs = (sql.prepare
				<< "SELECT DISTINCT m.default_bgm "
				<< "FROM " << db.makeTable("map_data") << " m "
				<< "WHERE m.default_bgm LIKE :q",
				soci::use(q, "q"));

			displayFunc(rs, format, matches[2]);
		}
		else if (rawType == "drops") {
			auto format = [](const soci::row &row, out_stream_t &str) {
				str << row.get<int32_t>(0) << " : " << row.get<string_t>(1) << " (base rate " << (static_cast<double>(row.get<int32_t>(2)) / 1000000. * 100.) << "%)";
			};

			string_t q = matches[2];
			if (q.empty()) {
				return requiresSecondArgument(rawType);
			}
			if (!isIntegerString(q)) {
				return shouldBeIdOnly("drops", q);
			}

			soci::rowset<> rs = (sql.prepare
				<< "SELECT d.itemid, s.label, d.chance "
				<< "FROM " << db.makeTable("drop_data") << " d "
				<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
				<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << db.makeTable("user_drop_data") << ") AND d.dropperid = :q "
				<< "UNION ALL "
				<< "SELECT d.itemid, s.label, d.chance "
				<< "FROM " << db.makeTable("user_drop_data") << " d "
				<< "INNER JOIN " << db.makeTable("strings") << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
				<< "WHERE d.dropperid = :q "
				<< "ORDER BY itemid",
				soci::use(q, "q"));

			displayFunc(rs, format, matches[2]);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid search type: " + rawType);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto InfoFunctions::pos(Player *player, const chat_t &args) -> ChatResult {
	ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) { message << "{X, Y} [Foothold]: " << player->getPos() << " [" << player->getFoothold() << "]"; });
	return ChatResult::HandledDisplay;
}

auto InfoFunctions::online(Player *player, const chat_t &args) -> ChatResult {
	out_stream_t igns;
	igns << "IGNs: ";
	int32_t i = 0;
	const int32_t max = 100;
	ChannelServer::getInstance().getPlayerDataProvider().run([&i, &max, &igns](Player *player) {
		if (i < max) {
			if (i != 0) {
				igns << ", ";
			}
			igns << player->getName();
			i++;
		}
	});
	ChatHandlerFunctions::showInfo(player, igns.str());
	return ChatResult::HandledDisplay;
}

auto InfoFunctions::variable(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+))", matches) == MatchResult::NoMatches) {
		return ChatResult::ShowSyntax;
	}

	string_t key = matches[1];
	string_t val = player->getVariables()->getVariable(key);
	if (!val.empty()) {
		ChatHandlerFunctions::showInfo(player, key + ": " + val);
	}
	else {
		ChatHandlerFunctions::showError(player, "Invalid variable: " + key);
	}
	return ChatResult::HandledDisplay;
}

auto InfoFunctions::questData(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) (\w+))", matches) == MatchResult::NoMatches) {
		return ChatResult::ShowSyntax;
	}

	string_t quest = matches[1];
	string_t data = matches[2];
	quest_id_t id = atoi(quest.c_str());
	player->getQuests()->setQuestData(id, data);
	return ChatResult::HandledDisplay;
}

auto InfoFunctions::questKills(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) (\d+))", matches) == MatchResult::NoMatches) {
		return ChatResult::ShowSyntax;
	}

	string_t mob = matches[1];
	string_t kills = matches[2];

	mob_id_t mobId = atoi(mob.c_str());
	int32_t count = atoi(kills.c_str());

	for (int32_t i = 0; i < count; i++) {
		player->getQuests()->updateQuestMob(mobId);
	}

	return ChatResult::HandledDisplay;
}

auto InfoFunctions::gmLevel(Player *player, const chat_t &args) -> ChatResult {
	ChatHandlerFunctions::showInfo(player, [&](chat_stream_t &message) { message << "Your GM level: " << player->getGmLevel(); });
	return ChatResult::HandledDisplay;
}