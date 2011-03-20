/*
Copyright (C) 2008-2011 Vana Development Team

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
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

namespace Functors {
	struct NameFunctor {
		void operator() (Player *player) {
			if (*i < max) {
				if (*i != 0) {
					*names = *names + ", ";
				}
				*names = *names + player->getName();
				(*i)++;
			}
		}
		int32_t max;
		int32_t *i;
		string *names;
	};
}

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
		string msg = "You may not use any commands.";
		bool has = false;
		for (unordered_map<string, ChatCommand>::iterator iter = CommandList.begin(); iter != CommandList.end(); iter++) {
			if (player->getGmLevel() >= iter->second.level) {
				if (!has) {
					msg = "Available commands: ";
					has = true;
				}
				msg += iter->first + " ";
			}
		}
		PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
	}
	return true;
}

bool InfoFunctions::lookup(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (.+)", matches)) {
		uint16_t type = 0;
		string test = matches[1];
		if (test == "item") type = 1;
		else if (test == "skill") type = 2;
		else if (test == "map") type = 3;
		else if (test == "mob") type = 4;
		else if (test == "npc") type = 5;
		else if (test == "quest") type = 6;

		else if (test == "id") type = 100;

		else if (test == "continent") type = 200;
		else if (test == "scriptbyname") type = 300;
		else if (test == "scriptbyid") type = 400;

		if (type != 0) {
			mysqlpp::Query query = Database::getDataDB().query();
			mysqlpp::StoreQueryResult res;

			string q = matches[2];
			if (type < 200) {
				if (type == 100) {
					query << "SELECT objectid, `label` FROM strings WHERE objectid = " << mysqlpp::quote << q;
				}
				else {
					query << "SELECT objectid, `label` FROM strings WHERE object_type = " << type << " AND label LIKE " << mysqlpp::quote << ("%" + q + "%") ;
				}
				res = query.store();

				if (res.num_rows() == 0) {
					PlayerPacket::showMessage(player, "No results.", PlayerPacket::NoticeTypes::Red);
				}
				else {
					for (size_t i = 0; i < res.num_rows(); i++) {
						string msg = (string) res[i][0] + " : " + (string) res[i][1];
						PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
					}
				}
			}
			else if (type == 200) {
				int32_t mapid = ChatHandlerFunctions::getMap(matches[2], player);
				if (Maps::getMap(mapid) != nullptr) {
					string message = lexical_cast<string>(mapid) + " : " + lexical_cast<string>((int32_t)(MapDataProvider::Instance()->getContinent(mapid)));
					PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Blue);
				}
				else {
					PlayerPacket::showMessage(player, "Invalid map.", PlayerPacket::NoticeTypes::Red);
				}
			}
			else if (type > 200) {
				if (type == 300) {
					query << "SELECT script_type, objectid, script FROM scripts WHERE script LIKE " << mysqlpp::quote << ("%" + q + "%");
				}
				else if (type == 400) {
					query << "SELECT script_type, objectid, script FROM scripts WHERE objectid = " << mysqlpp::quote << q;
				}
				res = query.store();

				if (res.num_rows() == 0) {
					PlayerPacket::showMessage(player, "No results.", PlayerPacket::NoticeTypes::Red);
				}
				else {
					for (size_t i = 0; i < res.num_rows(); i++) {
						string msg = (string) res[i][1] + " (" + (string) res[i][0] + "): " + (string) res[i][2];
						PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
					}
				}
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
	string msg = "(FH, X, Y): (";
	msg += lexical_cast<string>(player->getFh()) + ", ";
	msg += lexical_cast<string>(p.x) + ", ";
	msg += lexical_cast<string>(p.y) + ")";
	PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool InfoFunctions::online(Player *player, const string &args) {
	string igns = "IGNs: ";
	int32_t i = 0;
	Functors::NameFunctor func = {100, &i, &igns}; // Max of 100, may decide to change this in the future
	PlayerDataProvider::Instance()->run(func);
	PlayerPacket::showMessage(player, igns, PlayerPacket::NoticeTypes::Blue);
	return true;
}