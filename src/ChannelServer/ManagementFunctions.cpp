/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "ItemDataProvider.h"
#include "Maps.h"
#include "NpcHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "StringUtilities.h"
#include "SyncPacket.h"
#include "WorldServerConnectPacket.h"

using StringUtilities::lexical_cast;

bool ManagementFunctions::map(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t mapId = ChatHandlerFunctions::getMap(args, player);
		if (Maps::getMap(mapId)) {
			player->setMap(mapId);
		}
		else {
			PlayerPacket::showMessage(player, "Map not found: " + args, PlayerPacket::NoticeTypes::Red);
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
	SyncPacket::ConfigPacket::scrollingHeader(args);
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
	player->changeChannel((int8_t)ChannelServer::Instance()->getChannelId());
	return true;
}

bool ManagementFunctions::calculateRanks(Player *player, const string &args) {
	WorldServerConnectPacket::rankingCalculation();
	PlayerPacket::showMessage(player, "Sent a signal to force the calculation of rankings.", PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool ManagementFunctions::item(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\d+) ?(\\d*)?", matches)) {
		int32_t itemId = atoi(string(matches[1]).c_str());
		if (ItemDataProvider::Instance()->itemExists(itemId)) {
			string countString = matches[2];
			uint16_t count = countString.length() > 0 ? atoi(countString.c_str()) : 1;
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
		int32_t shopId = -1;
		if (args == "gear") shopId = 9999999;
		else if (args == "scrolls") shopId = 9999998;
		else if (args == "nx") shopId = 9999997;
		else if (args == "face") shopId = 9999996;
		else if (args == "ring") shopId = 9999995;
		else if (args == "chair") shopId = 9999994;
		else if (args == "mega") shopId = 9999993;
		else if (args == "pet") shopId = 9999992;
		else shopId = atoi(args.c_str());

		if (NpcHandler::showShop(player, shopId)) {
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
			WorldServerConnectPacket::reloadMcdb(args);
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
		int32_t npcId = atoi(args.c_str());
		Npc *npc = new Npc(npcId, player);
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
		int32_t id = player->getMap()->addNpc(npc);
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
	}
	else {
		bool proceed = true;
		auto iterate = [&player](function<bool(Player *p)> func) -> int {
			int32_t kills = 0;
			Map *map = player->getMap();
			for (size_t i = 0; i < map->getNumPlayers(); ++i) {
				Player *t = map->getPlayer(i);
				if (t != player) {
					if (func(t)) kills++;
				}
			}
			return kills;
		};
		if (args == "all") {
			proceed = false;
			int32_t kills = iterate([](Player *p) -> bool {
				p->getStats()->setHp(0);
				return true;
			});
			PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  players in the current map!", PlayerPacket::NoticeTypes::Blue);
		}
		else if (args == "gm" || args == "players") {
			proceed = false;
			int32_t kills = iterate([&args](Player *p) -> bool {
				if ((args == "gm" && p->isGm()) || (args == "players" && !p->isGm())) {
					p->getStats()->setHp(0);
					return true;
				}
				return false;
			});
			PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  " + (args == "gm" ? "GMs" : "players") + " in the current map!", PlayerPacket::NoticeTypes::Blue);
		}
		if (proceed) {
			if (args == "me") {
				player->getStats()->setHp(0);
				PlayerPacket::showMessage(player, "Killed yourself.", PlayerPacket::NoticeTypes::Blue);
			}
			else if (Player *kill = PlayerDataProvider::Instance()->getPlayer(args)) {
				// Kill by name
				kill->getStats()->setHp(0);
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
		string targetName = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetName)) {
			target->getSession()->disconnect();
		}
		string reasonString = matches[2];
		int8_t reason = reasonString.length() > 0 ? atoi(reasonString.c_str()) : 1;

		// Ban account
		string expire("2130-00-00 00:00:00");

		soci::session &sql = Database::getCharDb();
		soci::statement st = (sql.prepare
			<< "UPDATE user_accounts u "
			<< "INNER JOIN characters c ON u.user_id = c.user_id "
			<< "SET "
			<< "	u.banned = 1, "
			<< "	u.ban_expire = :expire, "
			<< "	u.ban_reason = :reason "
			<< "WHERE c.name = :name ",
			soci::use(targetName, "name"),
			soci::use(expire, "expire"),
			soci::use(reason, "reason"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			const string &banMessage = targetName + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			PlayerPacket::showMessageChannel(banMessage, PlayerPacket::NoticeTypes::Notice);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM banned a character with reason " + lexical_cast<string>(reason) + ". GM: " + player->getName() + ", Character: " + targetName);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't ban " + targetName + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}

		return true;
	}
	return false;
}

bool ManagementFunctions::tempBan(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (\\d+) (\\d+)", matches)) {
		string targetName = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetName)) {
			target->getSession()->disconnect();
		}
		string reasonString = matches[2];
		string length = matches[3];
		int8_t reason = reasonString.length() > 0 ? atoi(reasonString.c_str()) : 1;

		// Ban account
		soci::session &sql = Database::getCharDb();
		soci::statement st = (sql.prepare
			<< "UPDATE user_accounts u "
			<< "INNER JOIN characters c ON u.user_id = c.user_id "
			<< "SET "
			<< "	u.banned = 1, "
			<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY), "
			<< "	u.ban_reason = :reason "
			<< "WHERE c.name = :name ",
			soci::use(targetName, "name"),
			soci::use(length, "expire"),
			soci::use(reason, "reason"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			const string &banMessage = targetName + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			PlayerPacket::showMessageChannel(banMessage, PlayerPacket::NoticeTypes::Notice);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM temporary banned a character with reason " + lexical_cast<string>(reason) + " for " + length + " days. GM: " + player->getName() + ", Character: " + targetName);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't temporary ban " + targetName + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}

		return true;
	}
	return false;
}

bool ManagementFunctions::ipBan(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) ?(\\d+)?", matches)) {
		string targetName = matches[1];
		if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetName)) {
			const string &targetIp = target->getIp().toString();
			target->getSession()->disconnect();

			string reasonString = matches[2];
			int8_t reason = reasonString.length() > 0 ? atoi(reasonString.c_str()) : 1;

			// IP ban
			soci::session &sql = Database::getCharDb();
			soci::statement st = (sql.prepare << "INSERT INTO ip_bans (ip) VALUES (:ip)", soci::use(targetIp, "ip"));

			st.execute();

			if (st.get_affected_rows() > 0) {
				const string &banMessage = targetName + " has been IP banned" + ChatHandlerFunctions::getBanString(reason);
				PlayerPacket::showMessageChannel(banMessage, PlayerPacket::NoticeTypes::Notice);
				ChannelServer::Instance()->log(LogTypes::GmCommand, "GM IP banned a character with reason " + lexical_cast<string>(reason) + ". GM: " + player->getName() + ", Character: " + targetName);
			}
			else {
				PlayerPacket::showMessage(player, "Couldn't IP ban " + targetIp + ".", PlayerPacket::NoticeTypes::Red);
			}
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::unban(Player *player, const string &args) {
	if (args.length() != 0) {
		// Unban account
		soci::session &sql = Database::getCharDb();
		soci::statement st = (sql.prepare
			<< "UPDATE user_accounts u "
			<< "INNER JOIN characters c ON u.user_id = c.user_id "
			<< "SET "
			<< "	u.banned = 0, "
			<< "	u.ban_reason = NULL, "
			<< "	u.ban_expire = NULL "
			<< "WHERE c.name = :name ",
			soci::use(args, "name"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			const string &banMessage = args + " has been unbanned.";
			PlayerPacket::showMessage(player, banMessage, PlayerPacket::NoticeTypes::Blue);
			ChannelServer::Instance()->log(LogTypes::GmCommand, "GM unbanned a character. GM: " + player->getName() + ", Character: " + args);
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't unban " + args + ". Character not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool ManagementFunctions::rehash(Player *player, const string &args) {
	WorldServerConnectPacket::rehashConfig();
	PlayerPacket::showMessage(player, "Sent a signal to force rehashing world configurations.", PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool ManagementFunctions::rates(Player *player, const string &args) {
	cmatch matches;
	if (args.length() > 0) {
		if (!ChatHandlerFunctions::runRegexPattern(args, "(\\w+) ?(\\d+)?", matches)) {
			return false;
		}
		string type = matches[1];
		if (type == "view") {
			auto display = [=](const string &type, int32_t rate) {
				PlayerPacket::showMessage(player, type + " Rate: " + StringUtilities::lexical_cast<string>(rate) + "x", PlayerPacket::NoticeTypes::Blue);
			};
			PlayerPacket::showMessage(player, "Current Rates", PlayerPacket::NoticeTypes::Blue);
			display("Mob EXP", ChannelServer::Instance()->getMobExpRate());
			display("Mob Meso", ChannelServer::Instance()->getMobMesoRate());
			display("Quest EXP", ChannelServer::Instance()->getQuestExpRate());
			display("Drop", ChannelServer::Instance()->getDropRate());
		}
		else if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (\\d+)", matches)) {
			string value = matches[2];
			int32_t rateType = 0;
			int32_t newAmount = value.length() > 0 ? atoi(value.c_str()) : 1;

			if (type == "mobexp") rateType = Rates::Types::MobExpRate;
			if (type == "mobmeso") rateType = Rates::Types::MobMesoRate;
			if (type == "questexp") rateType = Rates::Types::QuestExpRate;
			if (type == "drop") rateType = Rates::Types::DropRate;
			ChannelServer::Instance()->modifyRate(rateType, newAmount);
			PlayerPacket::showMessage(player, "Sent request to modify rate", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			return false;
		}
	}
	else {
		PlayerPacket::showMessage(player, "Sent request to reset rates", PlayerPacket::NoticeTypes::Blue);
		SyncPacket::ConfigPacket::resetRates();
	}
	return true;
}