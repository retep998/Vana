/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Database.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include <iostream>

auto InfoFunctions::help(Player *player, const string_t &args) -> bool {
	using ChatHandlerFunctions::sCommandList;
	if (args.length() != 0) {
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
	return true;
}

auto InfoFunctions::lookup(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches)) {
		uint16_t type = 0;
		uint16_t subType = 0;

		string_t rawType = matches[1];
		// These constants correspond to MCDB enum types
		if (rawType == "item") type = 1;
		else if (rawType == "equip") { type = 1; subType = 1; }
		else if (rawType == "use") { type = 1; subType = 2; }
		else if (rawType == "etc") { type = 1; subType = 4; }
		else if (rawType == "cash") { type = 1; subType = 5; }
		else if (rawType == "skill") type = 2;
		else if (rawType == "map") type = 3;
		else if (rawType == "mob") type = 4;
		else if (rawType == "npc") type = 5;
		else if (rawType == "quest") type = 6;
		// The rest of the constants don't, they're merely there for later processing
		else if (rawType == "id") type = 100;
		else if (rawType == "continent") type = 200;
		else if (rawType == "scriptbyname") type = 300;
		else if (rawType == "scriptbyid") type = 400;
		else if (rawType == "whatdrops") type = 500;
		else if (rawType == "whatmaps") type = 600;
		else if (rawType == "music") type = 700;
		else if (rawType == "drops") type = 800;

		if (type != 0) {
			soci::session &sql = Database::getDataDb();
			auto displayFunc = [&sql, &player](const soci::rowset<> &rs, function_t<void(const soci::row &row, out_stream_t &str)> formatMessage) {
				// Bug in the behavior of SOCI
				// In the case where you use dynamic resultset binding, got_data() will not properly report that it got results

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

			string_t q = matches[2];
			if (type < 200) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
				};

				if (type == 100) {
					soci::rowset<> rs = (sql.prepare << "SELECT objectid, `label` FROM " << Database::makeDataTable("strings") << " WHERE objectid = :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
				else if (type == 1 && subType != 0) {
					q = "%" + q + "%";
					soci::rowset<> rs = (sql.prepare
						<< "SELECT s.objectid, s.`label` "
						<< "FROM " << Database::makeDataTable("strings") << " s "
						<< "INNER JOIN " << Database::makeDataTable("item_data") << " i ON s.objectid = i.itemid "
						<< "WHERE s.object_type = :type AND s.label LIKE :q AND i.inventory = :subtype",
						soci::use(q, "q"),
						soci::use(type, "type"),
						soci::use(subType, "subtype"));

					displayFunc(rs, format);
				}
				else {
					q = "%" + q + "%";
					soci::rowset<> rs = (sql.prepare
						<< "SELECT objectid, `label` "
						<< "FROM " << Database::makeDataTable("strings") << " "
						<< "WHERE object_type = :type AND label LIKE :q",
						soci::use(q, "q"),
						soci::use(type, "type"));

					displayFunc(rs, format);
				}
			}
			else if (type == 200) {
				string_t rawMap = matches[2];
				int32_t mapId = ChatHandlerFunctions::getMap(rawMap, player);
				if (Maps::getMap(mapId) != nullptr) {
					out_stream_t message;
					message << mapId << " : " << static_cast<int32_t>(MapDataProvider::getInstance().getContinent(mapId));
					ChatHandlerFunctions::showInfo(player, message.str());
				}
				else {
					ChatHandlerFunctions::showError(player, "Invalid map: " + rawMap);
				}
			}
			else if (type == 300 || type == 400) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(1) << " (" << row.get<string_t>(0) << "): " << row.get<string_t>(2);
				};

				if (type == 300) {
					q = "%" + q + "%";
					soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << Database::makeDataTable("scripts") << " WHERE script LIKE :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
				else if (type == 400) {
					soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM " << Database::makeDataTable("scripts") << " WHERE objectid = :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
			}
			else if (type == 500) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT d.dropperid, s.label "
					<< "FROM " << Database::makeDataTable("drop_data") << " d "
					<< "INNER JOIN " << Database::makeDataTable("strings") << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
					<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << Database::makeDataTable("user_drop_data") << ") AND d.itemid = :q "
					<< "UNION ALL "
					<< "SELECT d.dropperid, s.label "
					<< "FROM " << Database::makeDataTable("user_drop_data") << " d "
					<< "INNER JOIN " << Database::makeDataTable("strings") << " s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
					<< "WHERE d.itemid = :q ",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 600) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string_t>(1);
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT m.mapid, s.label "
					<< "FROM " << Database::makeDataTable("map_data") << " m "
					<< "INNER JOIN " << Database::makeDataTable("strings") << " s ON s.objectid = m.mapid AND s.object_type = 'map' "
					<< "WHERE m.mapid IN (SELECT ml.mapid FROM " << Database::makeDataTable("map_life") << " ml WHERE ml.lifeid = :q AND ml.life_type = 'mob') ",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 700) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<string_t>(0);
				};

				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT DISTINCT m.default_bgm "
					<< "FROM " << Database::makeDataTable("map_data") << " m "
					<< "WHERE m.default_bgm LIKE :q",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 800) {
				auto format = [](const soci::row &row, out_stream_t &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string_t>(1) << " (base rate " << (static_cast<double>(row.get<int32_t>(2)) / 1000000. * 100.) << "%)";
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT d.itemid, s.label, d.chance "
					<< "FROM " << Database::makeDataTable("drop_data") << " d "
					<< "INNER JOIN " << Database::makeDataTable("strings") << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
					<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM " << Database::makeDataTable("user_drop_data") << ") AND d.dropperid = :q "
					<< "UNION ALL "
					<< "SELECT d.itemid, s.label, d.chance "
					<< "FROM " << Database::makeDataTable("user_drop_data") << " d "
					<< "INNER JOIN " << Database::makeDataTable("strings") << " s ON s.objectid = d.itemid AND s.object_type = 'item' "
					<< "WHERE d.dropperid = :q "
					<< "ORDER BY itemid",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid search type: " + rawType);
		}
		return true;
	}
	return false;
}

auto InfoFunctions::pos(Player *player, const string_t &args) -> bool {
	Pos p = player->getPos();
	out_stream_t msg;
	msg << "(Foothold, {X, Y}): (" << player->getFoothold() << ", " << p << ")";
	ChatHandlerFunctions::showInfo(player, msg.str());
	return true;
}

auto InfoFunctions::online(Player *player, const string_t &args) -> bool {
	out_stream_t igns;
	igns << "IGNs: ";
	int32_t i = 0;
	const int32_t max = 100;
	PlayerDataProvider::getInstance().run([&i, &max, &igns](Player *player) {
		if (i < max) {
			if (i != 0) {
				igns << ", ";
			}
			igns << player->getName();
			i++;
		}
	});
	ChatHandlerFunctions::showInfo(player, igns.str());
	return true;
}

auto InfoFunctions::variable(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (!ChatHandlerFunctions::runRegexPattern(args, R"((\w+))", matches)) {
		return false;
	}

	string_t key = matches[1];
	string_t val = player->getVariables()->getVariable(key);
	if (!val.empty()) {
		ChatHandlerFunctions::showInfo(player, key + ": " + val);
	}
	else {
		ChatHandlerFunctions::showError(player, "Invalid variable: " + key);
	}
	return true;
}

auto InfoFunctions::questData(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (!ChatHandlerFunctions::runRegexPattern(args, R"((\d+) (\w+))", matches)) {
		return false;
	}

	string_t quest = matches[1];
	string_t data = matches[2];
	uint16_t id = atoi(quest.c_str());
	player->getQuests()->setQuestData(id, data);
	return true;
}

auto InfoFunctions::questKills(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (!ChatHandlerFunctions::runRegexPattern(args, R"((\d+) (\d+))", matches)) {
		return false;
	}

	string_t mob = matches[1];
	string_t kills = matches[2];

	int32_t mobId = atoi(mob.c_str());
	int32_t count = atoi(kills.c_str());

	for (int32_t i = 0; i < count; i++) {
		player->getQuests()->updateQuestMob(mobId);
	}

	return true;
}