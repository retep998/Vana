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
#include "ManagementFunctions.h"
#include "ChannelServer.h"
#include "Database.h"
#include "Inventory.h"
#include "IpUtilities.h"
#include "ItemDataProvider.h"
#include "Maps.h"
#include "NpcHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "WorldServerConnectPacket.h"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

bool ManagementFunctions::map(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t mapid = ChatHandlerFunctions::getMap(args, player);
		if (Maps::getMap(mapid)) {
			player->setMap(mapid);
		}
		else {
			PlayerPacket::showMessage(player, "Map not found.", PlayerPacket::NoticeTypes::Red);
		}
	}
	else {
		PlayerPacket::showMessage(player, "Current Map: " + lexical_cast<string>(player->getMap()), PlayerPacket::NoticeTypes::Blue);
	}
	return true;
}

bool ManagementFunctions::changeChannel(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\d+)", matches)) {
		string targetChannel = matches[1];
		int8_t channel = atoi(targetChannel.c_str()) - 1;
		player->changeChannel(channel);
		return true;
	}
	return false;
}

bool ManagementFunctions::lag(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+)", matches)) {
		string target = matches[1];
		if (Player *p = PlayerDataProvider::Instance()->getPlayer(target)) {
			PlayerPacket::showMessage(player, p->getName() + "'s lag: " + lexical_cast<string>(p->getLatency()) + "ms", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			PlayerPacket::showMessage(player, "Player not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::header(Player *player, const string &args) {
	WorldServerConnectPacket::scrollingHeader(ChannelServer::Instance()->getWorldConnection(), args);
	return true;
}

bool ManagementFunctions::shutdown(Player *player, const string &args) {
	PlayerPacket::showMessage(player, "Shutting down the server.", PlayerPacket::NoticeTypes::Blue);
	ChannelServer::Instance()->log(LogTypes::GmCommand, "GM shutdown the server. GM: " + player->getName());
	ChannelServer::Instance()->shutdown();
	return true;
}

bool ManagementFunctions::kick(Player *player, const string &args) {
	if (args.length() != 0) {
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(args)) {
			if (player->getGmLevel() > target->getGmLevel()) {
				target->getSession()->disconnect();
				PlayerPacket::showMessage(player, "Kicked " + args + " from the server.", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				PlayerPacket::showMessage(player, "Player outranks you.", PlayerPacket::NoticeTypes::Red);
			}
		}
		else {
			PlayerPacket::showMessage(player, "Invalid player or player is offline.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::relog(Player *player, const string &args) {
	player->changeChannel((int8_t)ChannelServer::Instance()->getChannel());
	return true;
}

bool ManagementFunctions::calculateRanks(Player *player, const string &args) {
	WorldServerConnectPacket::rankingCalculation(ChannelServer::Instance()->getWorldConnection());
	PlayerPacket::showMessage(player, "Sent a signal to force the calculation of rankings.", PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool ManagementFunctions::item(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\d+) ?(\\d*)?", matches)) {
		int32_t itemId = atoi(string(matches[1]).c_str());
		if (ItemDataProvider::Instance()->itemExists(itemId)) {
			string countstring = matches[2];
			uint16_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;
			Inventory::addNewItem(player, itemId, count);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid Item ID.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::storage(Player *player, const string &args) {
	NpcHandler::showStorage(player, 1012009);
	return true;
}

bool ManagementFunctions::shop(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t shopid = -1;
		if (args == "gear") shopid = 9999999;
		else if (args == "scrolls") shopid = 9999998;
		else if (args == "nx") shopid = 9999997;
		else if (args == "face") shopid = 9999996;
		else if (args == "ring") shopid = 9999995;
		else if (args == "chair") shopid = 9999994;
		else if (args == "mega") shopid = 9999993;
		else if (args == "pet") shopid = 9999992;
		else shopid = atoi(args.c_str());

		if (NpcHandler::showShop(player, shopid)) {
			return true;
		}
	}
	return false;
}

bool ManagementFunctions::reload(Player *player, const string &args) {
	if (args.length() != 0) {
		if (args == "items" || args == "drops" || args == "shops" ||
			args == "mobs" || args == "beauty" || args == "scripts" ||
			args == "skills" || args == "reactors" || args == "pets" ||
			args == "quests" || args == "all") {
			WorldServerConnectPacket::reloadMcdb(ChannelServer::Instance()->getWorldConnection(), args);
			PlayerPacket::showMessage(player, "Reloading message for " + args + " sent to all channels.", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid reload type.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::npc(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t npcid = atoi(args.c_str());
		Npc *npc = new Npc(npcid, player);
		npc->run();
		return true;
	}
	return false;
}

bool ManagementFunctions::addNpc(Player *player, const string &args) {
	if (args.length() != 0) {
		NpcSpawnInfo npc;
		npc.id = atoi(args.c_str());
		npc.foothold = 0;
		npc.pos = player->getPos();
		npc.rx0 = npc.pos.x - 50;
		npc.rx1 = npc.pos.x + 50;
		int32_t id = Maps::getMap(player->getMap())->addNpc(npc);
		PlayerPacket::showMessage(player, "Spawned NPC with object ID " + lexical_cast<string>(id) , PlayerPacket::NoticeTypes::Blue);
		return true;
	}
	return false;
}

bool ManagementFunctions::killNpc(Player *player, const string &args) {
	player->setNpc(nullptr);
	return true;
}

bool ManagementFunctions::kill(Player *player, const string &args) {
	if (player->getGmLevel() == 1) {
		player->getStats()->setHp(0);
		PlayerPacket::showMessage(player, "Killed yourself.", PlayerPacket::NoticeTypes::Blue);
	}
	else {
		bool proceed = true;
		if (args == "gm" || args == "all") {
			proceed = false;
			int32_t kills = 0;
			for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
				Player *killpsa = Maps::getMap(player->getMap())->getPlayer(i);
				if (killpsa != player) {
					if (killpsa->isGm()) {
						killpsa->getStats()->setHp(0);
						kills++;
					}
				}
			}
			PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  GMs in the current map!", PlayerPacket::NoticeTypes::Blue);
		}
		if (args == "players" || args == "all") {
			proceed = false;
			int32_t kills = 0;
			for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
				Player *killpsa = Maps::getMap(player->getMap())->getPlayer(i);
				if (killpsa != player) {
					if (!killpsa->isGm()) {
						killpsa->getStats()->setHp(0);
						kills++;
					}
				}
			}
			PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  players in the current map!", PlayerPacket::NoticeTypes::Blue);
		}
		if (proceed) {
			if (args == "me") {
				player->getStats()->setHp(0);
				PlayerPacket::showMessage(player, "Killed yourself.", PlayerPacket::NoticeTypes::Blue);
			}
			else if (Player *killpsa = PlayerDataProvider::Instance()->getPlayer(args)) {
				// Kill by name
				killpsa->getStats()->setHp(0);
				PlayerPacket::showMessage(player, "Killed " + args + ".", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				// Nothing valid
				return false;
			}
		}
	}
	return true;
}

bool ManagementFunctions::ban(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) ?(\\d+)?", matches)) {
		string targetname = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname)) {
			target->getSession()->disconnect();
		}
		string reasonstring = matches[2];
		int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

		// Ban account
		mysqlpp::Query accbanquery = Database::getCharDB().query();
		accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = '9000-00-00 00:00:00' WHERE characters.name = '" << targetname << "'";
		accbanquery.exec();

		int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
		if (affects != 0) {
			string banmsg = targetname + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM banned a character with reason " + lexical_cast<string>((int16_t)reason) + ". GM: " + player->getName() + ", Character: " + targetname);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't ban " + targetname + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::tempBan(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (\\d+) (\\d+)", matches)) {
		string targetname = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname)) {
			target->getSession()->disconnect();
		}
		string reasonstring = matches[2];
		string length = matches[3];
		int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

		// Ban account
		mysqlpp::Query accbanquery = Database::getCharDB().query();
		accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = DATE_ADD(NOW(), INTERVAL " << length << " DAY) WHERE characters.name = '" << targetname << "'";
		accbanquery.exec();

		int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
		if (affects != 0) {
			string banmsg = targetname + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM temporary banned a character with reason " + lexical_cast<string>((int16_t)reason) + " for " + length + " days. GM: " + player->getName() + ", Character: " + targetname);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't temporary ban " + targetname + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::ipBan(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) ?(\\d+)?", matches)) {
		string targetname = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname)) {
			string targetip = IpUtilities::ipToString(target->getIp());
			target->getSession()->disconnect();

			string reasonstring = matches[2];
			int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

			// Ip ban
			mysqlpp::Query accipbanquery = Database::getCharDB().query();
			accipbanquery << "INSERT INTO `ipbans`(`id`, `ip`) VALUES (NULL, '" << targetip << "')";
			accipbanquery.exec();

			int32_t affects = static_cast<int32_t>(accipbanquery.affected_rows());
			if (affects != 0) {
				string banmsg = targetname + " has been IP banned" + ChatHandlerFunctions::getBanString(reason);
				PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
				ChannelServer::Instance()->log(LogTypes::GmCommand, "GM IP banned a character with reason " + lexical_cast<string>((int16_t)reason) + ". GM: " + player->getName() + ", Character: " + targetname);
			}
			else {
				PlayerPacket::showMessage(player, "Couldn't IP ban " + targetip + ".", PlayerPacket::NoticeTypes::Red);
			}
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::unban(Player *player, const string &args) {
	if (args.length() != 0) {
		// Unban account
		mysqlpp::Query accbanquery = Database::getCharDB().query();
		accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_expire = '0000-00-00 00:00:00' WHERE characters.name = '" << args << "'";
		accbanquery.exec();

		int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
		if (affects != 0) {
			string banmsg = args + " has been unbanned.";
			PlayerPacket::showMessage(player, banmsg, PlayerPacket::NoticeTypes::Blue);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM unbanned a character. GM: " + player->getName() + ", Character: " + args);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't unban " + args + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}