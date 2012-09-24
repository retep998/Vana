/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "InfoFunctions.h"
#include "Database.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include <iostream>

bool InfoFunctions::help(Player *player, const string &args) {
	using ChatHandlerFunctions::CommandList;
	if (args.length() != 0) {
		if (CommandList.find(args) != CommandList.end()) {
			ChatHandlerFunctions::showSyntax(player, args, true);
		}
		else {
			PlayerPacket::showMessage(player, "Command \"" + args + "\" does not exist.", PlayerPacket::NoticeTypes::Red);
		}
	}
	else {
		bool has = false;
		std::ostringstream strm;
		strm << "You may not use any commands.";
		for (unordered_map<string, ChatCommand>::iterator iter = CommandList.begin(); iter != CommandList.end(); ++iter) {
			if (player->getGmLevel() >= iter->second.level) {
				if (!has) {
					strm.str("");
					strm.clear();
					strm << "Available commands: ";
					has = true;
				}
				strm << iter->first << " ";
			}
		}
		PlayerPacket::showMessage(player, strm.str(), PlayerPacket::NoticeTypes::Blue);
	}
	return true;
}

bool InfoFunctions::lookup(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (.+)", matches)) {
		uint16_t type = 0;
		string test = matches[1];
		// These constants correspond to MCDB enum types
		if (test == "item") type = 1;
		else if (test == "skill") type = 2;
		else if (test == "map") type = 3;
		else if (test == "mob") type = 4;
		else if (test == "npc") type = 5;
		else if (test == "quest") type = 6;
		// The rest of the constants don't, they're merely there for later processing
		else if (test == "id") type = 100;
		else if (test == "continent") type = 200;
		else if (test == "scriptbyname") type = 300;
		else if (test == "scriptbyid") type = 400;
		else if (test == "whatdrops") type = 500;
		else if (test == "whatmaps") type = 600;
		else if (test == "music") type = 700;
		else if (test == "drops") type = 800;

		if (type != 0) {
			soci::session &sql = Database::getDataDb();
			auto displayFunc = [&sql, &player](const soci::rowset<> &rs, function<void(const soci::row &row, std::ostringstream &str)> formatMessage) {
				// Bug in the behavior of SOCI
				// In the case where you use dynamic resultset binding, got_data() will not properly report that it got results

				std::ostringstream str("");
				bool found = false;
				for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
					const soci::row &row = *i;
					found = true;

					str.str("");
					str.clear();
					formatMessage(row, str);
					PlayerPacket::showMessage(player, str.str(), PlayerPacket::NoticeTypes::Blue);
				}

				if (!found) {
					PlayerPacket::showMessage(player, "No results.", PlayerPacket::NoticeTypes::Red);
				}
			};

			string q = matches[2];
			if (type < 200) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string>(1);
				};

				if (type == 100) {
					soci::rowset<> rs = (sql.prepare << "SELECT objectid, `label` FROM strings WHERE objectid = :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
				else {
					q = "%" + q + "%";
					soci::rowset<> rs = (sql.prepare
						<< "SELECT objectid, `label` "
						<< "FROM strings "
						<< "WHERE object_type = :type AND label LIKE :q",
						soci::use(q, "q"),
						soci::use(type, "type"));

					displayFunc(rs, format);
				}
			}
			else if (type == 200) {
				int32_t mapId = ChatHandlerFunctions::getMap(matches[2], player);
				if (Maps::getMap(mapId) != nullptr) {
					std::ostringstream message;
					message << mapId << " : " << static_cast<int32_t>(MapDataProvider::Instance()->getContinent(mapId));
					PlayerPacket::showMessage(player, message.str(), PlayerPacket::NoticeTypes::Blue);
				}
				else {
					PlayerPacket::showMessage(player, "Invalid map.", PlayerPacket::NoticeTypes::Red);
				}
			}
			else if (type == 300 || type == 400) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<int32_t>(1) << " (" << row.get<string>(0) << "): " << row.get<string>(2);
				};

				if (type == 300) {
					q = "%" + q + "%";
					soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM scripts WHERE script LIKE :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
				else if (type == 400) {
					soci::rowset<> rs = (sql.prepare << "SELECT script_type, objectid, script FROM scripts WHERE objectid = :q", soci::use(q, "q"));
					displayFunc(rs, format);
				}
			}
			else if (type == 500) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string>(1);
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT d.dropperid, s.label "
					<< "FROM drop_data d "
					<< "INNER JOIN strings s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
					<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM user_drop_data) AND d.itemid = :q "
					<< "UNION ALL "
					<< "SELECT d.dropperid, s.label "
					<< "FROM user_drop_data d "
					<< "INNER JOIN strings s ON s.objectid = d.dropperid AND s.object_type = 'mob' "
					<< "WHERE d.itemid = :q ",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 600) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string>(1);
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT m.mapid, s.label "
					<< "FROM map_data m "
					<< "INNER JOIN strings s ON s.objectid = m.mapid AND s.object_type = 'map' "
					<< "WHERE m.mapid IN (SELECT ml.mapid FROM map_life ml WHERE ml.lifeid = :q AND ml.life_type = 'mob') ",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 700) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<string>(0);
				};

				q = "%" + q + "%";
				soci::rowset<> rs = (sql.prepare
					<< "SELECT DISTINCT m.default_bgm "
					<< "FROM map_data m "
					<< "WHERE m.default_bgm LIKE :q",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
			else if (type == 800) {
				auto format = [](const soci::row &row, std::ostringstream &str) {
					str << row.get<int32_t>(0) << " : " << row.get<string>(1);
				};

				soci::rowset<> rs = (sql.prepare
					<< "SELECT d.itemid, s.label "
					<< "FROM drop_data d "
					<< "INNER JOIN strings s ON s.objectid = d.itemid AND s.object_type = 'item' "
					<< "WHERE d.dropperid NOT IN (SELECT DISTINCT dropperid FROM user_drop_data) AND d.dropperid = :q "
					<< "UNION ALL "
					<< "SELECT d.itemid, s.label "
					<< "FROM user_drop_data d "
					<< "INNER JOIN strings s ON s.objectid = d.itemid AND s.object_type = 'item' "
					<< "WHERE d.dropperid = :q "
					<< "ORDER BY itemid",
					soci::use(q, "q"));

				displayFunc(rs, format);
			}
		}
		else {
			PlayerPacket::showMessage(player, "Invalid search type - valid options are: {item, skill, map, mob, npc, quest, continent, id, scriptbyname, scriptbyid}", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool InfoFunctions::pos(Player *player, const string &args) {
	Pos p = player->getPos();
	std::ostringstream msg;
	msg << "(FH, X, Y): (" << player->getFh() << ", " << p.x << ", " << p.y << ")";
	PlayerPacket::showMessage(player, msg.str(), PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool InfoFunctions::online(Player *player, const string &args) {
	std::ostringstream igns;
	igns << "IGNs: ";
	int32_t i = 0;
	const int32_t max = 100;
	PlayerDataProvider::Instance()->run([&i, &max, &igns](Player *player) {
		if (i < max) {
			if (i != 0) {
				igns << ", ";
			}
			igns << player->getName();
			i++;
		}
	});
	PlayerPacket::showMessage(player, igns.str(), PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool InfoFunctions::variable(Player *player, const string &args) {
	cmatch matches;
	if (!ChatHandlerFunctions::runRegexPattern(args, "(\\w+)", matches)) {
		return false;
	}

	string test = matches[1];
	string val = player->getVariables()->getVariable(test);
	if (!val.empty()) {
		PlayerPacket::showMessage(player, test + ": " + val, PlayerPacket::NoticeTypes::Blue);
	}
	else {
		PlayerPacket::showMessage(player, "Variable '" + test + "' did not exist or has a blank value", PlayerPacket::NoticeTypes::Red);
	}
	return true;
}