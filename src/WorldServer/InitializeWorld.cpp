/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "InitializeWorld.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "Alliances.h"
#include "BbsHandler.h"
#include "Guilds.h"
#include "Players.h"

using Initializing::outputWidth;

void Initializing::loadData() {
	// Nothing for now
}

void Initializing::loadPostAssignment(int16_t worldId) {
	loadGuilds(worldId);
	loadAlliances(worldId);
	assignPlayers(worldId);
}

void Initializing::loadGuilds(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Guilds... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM guilds WHERE world = " << worldId << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		Guilds::Instance()->createGuild((string) res[i]["name"], 
			(string) res[i]["notice"], 
			res[i]["id"], 
			res[i]["leaderid"], 
			res[i]["capacity"], 
			static_cast<int16_t>(res[i]["logo"]), 
			static_cast<uint8_t>(res[i]["logocolor"]), 
			static_cast<int16_t>(res[i]["logobg"]), 
			static_cast<uint8_t>(res[i]["logobgcolor"]), 
			res[i]["gp"], 
			(string) res[i]["rank1title"], 
			(string) res[i]["rank2title"], 
			(string) res[i]["rank3title"], 
			(string) res[i]["rank4title"], 
			(string) res[i]["rank5title"], 
			res[i]["alliance"]);

		if ((int32_t) res[i]["alliance"] > 0) {
			Alliance *alliance = Alliances::Instance()->getAlliance(res[i]["alliance"]);
			if (alliance != 0)
				alliance->addGuild(Guilds::Instance()->getGuild(res[i]["id"]));
		}
	}

	std::cout << "DONE" << std::endl;
}

void Initializing::loadAlliances(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Alliances... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM alliances WHERE worldid = " << worldId << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		Alliances::Instance()->addAlliance(res[i]["id"], 
			(string) res[i]["name"], 
			(string) res[i]["notice"],
			(string) res[i]["rank1title"], 
			(string) res[i]["rank2title"], 
			(string) res[i]["rank3title"], 
			(string) res[i]["rank4title"], 
			(string) res[i]["rank5title"],
			res[i]["capacity"],
			res[i]["leader"]);
	}

	std::cout << "DONE" << std::endl;
}

void Initializing::assignPlayers(int16_t worldId) {
	std::cout << std::setw(outputWidth) << std::left << "Assigning Characters... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM characters WHERE guild <> 0 AND world_id = " << worldId << " ORDER BY guildrank ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		Guild * guild = Guilds::Instance()->getGuild(atoi(res[i]["guild"]));
		if (guild == 0) {
			std::cout << (string) res[i]["name"] << " has an invalid guildid (guild doesn't exist). Please check this! ";
			continue;
		}

		Players::Instance()->registerPlayer(0,
			res[i]["id"],
			(string) res[i]["name"],
			0,
			-1,
			-1,
			-1,
			res[i]["guild"],
			static_cast<uint8_t>(res[i]["guildrank"]), 
			res[i]["alliance"],
			static_cast<uint8_t>(res[i]["alliancerank"]), 
			false);
	}

	std::cout << "DONE" << std::endl;
}