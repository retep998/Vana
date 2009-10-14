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
#include "WorldServer.h"
#include "Alliances.h"
#include "BbsHandler.h"
#include "ConnectionManager.h"
#include "Database.h"
#include "Guilds.h"
#include "Players.h"
#include "InitializeCommon.h"
#include "IpUtilities.h"
#include "Types.h"
#include "WorldServerAcceptPacket.h"
#include <string>

using Initializing::outputWidth;

WorldServer * WorldServer::singleton = 0;

void WorldServer::listen() {
	ConnectionManager::Instance()->accept(inter_port, new WorldServerAcceptConnectionFactory());
}

void WorldServer::loadData() {
	loginPlayer = new LoginServerConnection;
	ConnectionManager::Instance()->connect(login_ip, login_inter_port, loginPlayer);
	loginPlayer->sendAuth(inter_password, external_ip);
}

void WorldServer::loadConfig() {
	ConfigFile config("conf/worldserver.lua");
	login_ip = IpUtilities::stringToIp(config.getString("login_ip"));
	login_inter_port = config.getShort("login_inter_port");

	inter_port = -1; // Will get from login server later
	scrollingHeader = ""; // Will get from login server later
}

void WorldServer::setScrollingHeader(const string &message) {
	scrollingHeader = message;
	WorldServerAcceptPacket::scrollingHeader(message);
}

void WorldServer::loadGuilds() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Alliances... ";

	Database::connectCharDB();
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM alliances WHERE worldid = " << static_cast<int16_t>(worldId) << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();

	// Load all alliances
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
	std::cout << std::setw(outputWidth) << std::left << "Initializing Guilds... ";

	query << "SELECT * FROM guilds WHERE world = " << static_cast<int16_t>(worldId) << " ORDER BY id ASC";
	res = query.store();

	// Load all guilds
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
	std::cout << std::setw(outputWidth) << std::left << "Initializing Characters... ";

	// Load all characters
	query << "SELECT * FROM characters WHERE guild <> 0 AND world_id = " << static_cast<int16_t>(worldId) << " ORDER BY guildrank ASC";
	res = query.store();

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
