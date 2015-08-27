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
#include "ManagementFunctions.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "ExitCodes.hpp"
#include "Inventory.hpp"
#include "ItemConstants.hpp"
#include "ItemDataProvider.hpp"
#include "Maps.hpp"
#include "MysticDoor.hpp"
#include "NpcHandler.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "RatesConfig.hpp"
#include "StringUtilities.hpp"
#include "SyncPacket.hpp"
#include "WorldServerConnectPacket.hpp"

auto ManagementFunctions::map(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+)? ?(\w+|\{(-?\d+)\, ?(-?\d+)\}|\[\d+\])?)", matches) == MatchResult::AnyMatches) {
		string_t rawMap = matches[1];
		string_t rawPortal = matches[2];
		if (rawMap.empty()) {
			ChatHandlerFunctions::showInfo(player, "Current map: " + StringUtilities::lexical_cast<string_t>(player->getMapId()));
			return ChatResult::HandledDisplay;
		}

		map_id_t mapId = ChatHandlerFunctions::getMap(rawMap, player);
		if (mapId != -1 && Maps::getMap(mapId) != nullptr) {
			Map *map = Maps::getMap(mapId);
			// We determine here if we're looking for a position or a portal
			if (!rawPortal.empty() && ChatHandlerFunctions::runRegexPattern(rawPortal, R"(\{(-?\d+)\, ?(-?\d+)\})", matches) == MatchResult::AnyMatches) {
				string_t xPosition = matches[1];
				string_t yPosition = matches[2];
				Point pos{
					StringUtilities::lexical_cast<coord_t>(xPosition),
					StringUtilities::lexical_cast<coord_t>(yPosition)
				};
				player->setMap(mapId, MysticDoor::PortalId, pos);
			}
			else if (!rawPortal.empty() && ChatHandlerFunctions::runRegexPattern(rawPortal, R"(\[(\d+)\])", matches) == MatchResult::AnyMatches) {
				string_t footholdId = matches[1];
				foothold_id_t foothold = StringUtilities::lexical_cast<foothold_id_t>(footholdId);
				if (!map->isValidFoothold(foothold) || map->isVerticalFoothold(foothold)) {
					ChatHandlerFunctions::showError(player, "Invalid foothold: " + footholdId);
					return ChatResult::HandledDisplay;
				}
				Point pos = map->getPositionAtFoothold(foothold);
				player->setMap(mapId, MysticDoor::PortalId, pos);
			}
			else {
				const PortalInfo * const destinationPortal = map->queryPortalName(rawPortal);

				if (!rawPortal.empty() && destinationPortal == nullptr) {
					ChatHandlerFunctions::showError(player, "Invalid portal: " + rawPortal);
					return ChatResult::HandledDisplay;
				}

				if (rawPortal == "tp") {
					player->setMap(mapId, destinationPortal->id, destinationPortal->pos);
				}
				else {
					player->setMap(mapId, destinationPortal);
				}
			}
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid map: " + rawMap);
		}

		return ChatResult::HandledDisplay;
	}

	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::warp(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (\w+) (\w+) ?(\w+)? ?(\w+)?)", matches) == MatchResult::AnyMatches) {
		string_t sourceType = matches[1];
		string_t destinationType = matches[2];
		string_t rawMap = matches[3];
		string_t optional = matches[4];
		string_t moreOptional = matches[5];
		map_id_t sourceMapId = -1;
		map_id_t destinationMapId = -1;
		string_t portal;
		Player *sourcePlayer = nullptr;
		bool validCombo = true;
		bool onlySource = true;
		bool singleArgumentDestination = false;
		bool portalSpecified = false;

		auto resolvePlayer = [](const string_t &playerArg) { return ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerArg); };
		auto resolveMapArg = [player](const string_t &mapArg) { return ChatHandlerFunctions::getMap(mapArg, player); };
		auto resolveMapPortal = [&portal, player](const string_t &portalArg) {
			if (!portalArg.empty()) {
				portal = portalArg;
				return true;
			}
			return false;
		};
		auto resolveMapCurrent = [player]() { return player->getMapId(); };
		auto resolveMapPlayer = [player](const string_t &playerArg) {
			if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerArg)) {
				return target->getMapId();
			}
			return -2;
		};

		if (sourceType == "map") {
			onlySource = false;

			sourceMapId = resolveMapArg(rawMap);
			if (sourceMapId == -1 || Maps::getMap(sourceMapId) == nullptr) {
				ChatHandlerFunctions::showError(player, "Invalid source map: " + rawMap);
				return ChatResult::HandledDisplay;
			}

			if (destinationType == "map") {
				destinationMapId = resolveMapArg(optional);
				portalSpecified = resolveMapPortal(moreOptional);
			}
			else if (destinationType == "current" || destinationType == "self") {
				destinationMapId = resolveMapCurrent();
				portalSpecified = resolveMapPortal(optional);
			}
			else if (destinationType == "player") {
				destinationMapId = resolveMapPlayer(optional);
				portalSpecified = resolveMapPortal(moreOptional);
			}
			else {
				validCombo = false;
			}
		}
		else if (sourceType == "player") {
			onlySource = false;

			sourcePlayer = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(rawMap);
			if (sourcePlayer == nullptr) {
				ChatHandlerFunctions::showError(player, "Invalid source player: " + rawMap);
				return ChatResult::HandledDisplay;
			}

			if (destinationType == "map") {
				destinationMapId = resolveMapArg(optional);
				portalSpecified = resolveMapPortal(moreOptional);
			}
			else if (destinationType == "current" || destinationType == "self") {
				destinationMapId = resolveMapCurrent();
				portalSpecified = resolveMapPortal(optional);
			}
			else if (destinationType == "player") {
				destinationMapId = resolveMapPlayer(optional);
				portalSpecified = resolveMapPortal(moreOptional);
			}
			else {
				validCombo = false;
			}
		}
		else if (sourceType == "current" || sourceType == "self") {
			sourceMapId = resolveMapCurrent();
			if (destinationType == "map") {
				destinationMapId = resolveMapArg(rawMap);
				portalSpecified = resolveMapPortal(optional);
			}
			else if (destinationType == "player") {
				destinationMapId = resolveMapPlayer(rawMap);
				portalSpecified = resolveMapPortal(optional);
			}
			else {
				validCombo = false;
			}
		}
		else if (sourceType == "channel") {
			if (destinationType == "map") {
				destinationMapId = resolveMapArg(rawMap);
				portalSpecified = resolveMapPortal(optional);
			}
			else if (destinationType == "current" || destinationType == "self") {
				destinationMapId = resolveMapCurrent();
				portalSpecified = resolveMapPortal(rawMap);
				singleArgumentDestination = true;
			}
			else if (destinationType == "player") {
				destinationMapId = resolveMapPlayer(rawMap);
				portalSpecified = resolveMapPortal(optional);
			}
			else {
				validCombo = false;
			}
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid source type: " + sourceType);
			return ChatResult::HandledDisplay;
		}

		if (!validCombo) {
			ChatHandlerFunctions::showError(player, "Invalid destination type for source type \"" + sourceType + "\": " + destinationType);
			return ChatResult::HandledDisplay;
		}

		if (destinationMapId == -2) {
			ChatHandlerFunctions::showError(player, "Invalid destination player: " + (onlySource ? rawMap : optional));
			return ChatResult::HandledDisplay;
		}

		if (destinationMapId == -1 || Maps::getMap(destinationMapId) == nullptr) {
			ChatHandlerFunctions::showError(player, "Invalid destination map: " + (onlySource ? rawMap : optional));
			return ChatResult::HandledDisplay;
		}

		Map *destination = Maps::getMap(destinationMapId);
		const PortalInfo * const destinationPortal = destination->queryPortalName(portal);

		if (portalSpecified && destinationPortal == nullptr) {
			ChatHandlerFunctions::showError(player, "Invalid destination portal: " + (singleArgumentDestination ? rawMap : (onlySource ? optional : moreOptional)));
			return ChatResult::HandledDisplay;
		}

		auto warpToMap = [&](Player *target) {
			if (target->getMapId() != destinationMapId) {
				if (portal == "tp") {
					target->setMap(destinationMapId, destinationPortal->id, destinationPortal->pos);
				}
				else {
					target->setMap(destinationMapId, destinationPortal);
				}
			}
		};

		if (sourceType == "map" || sourceType == "current") {
			out_stream_t message;
			message << "Warped all players on map ID " << sourceMapId << " to map ID " << destinationMapId;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			Maps::getMap(sourceMapId)->runFunctionPlayers(warpToMap);
			ChatHandlerFunctions::showInfo(player, message.str());
		}
		else if (sourceType == "player") {
			out_stream_t message;
			message << "Warped player " << rawMap << " to map ID " << destinationMapId;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			warpToMap(sourcePlayer);
			ChatHandlerFunctions::showInfo(player, message.str());
		}
		else if (sourceType == "self") {
			warpToMap(player);
		}
		else if (sourceType == "channel") {
			out_stream_t message;
			message << "Warped everyone in the channel to map ID " << destinationMapId;
			if (!portal.empty()) {
				message << " (portal " << portal << ")";
			}

			ChannelServer::getInstance().getPlayerDataProvider().run(warpToMap);
			ChatHandlerFunctions::showInfo(player, message.str());
		}

		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::follow(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+)?)", matches) == MatchResult::AnyMatches) {
		string_t playerName = matches[1];
		if (Player *follow = player->getFollow()) {
			if (!playerName.empty()) {
				ChatHandlerFunctions::showError(player, "You're already following player " + follow->getName());
			}
			else {
				ChannelServer::getInstance().getPlayerDataProvider().stopFollowing(player);
				ChatHandlerFunctions::showInfo(player, "No longer following " + follow->getName());
			}
		}
		else {
			if (playerName.size() != 0) {
				if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerName)) {
					ChannelServer::getInstance().getPlayerDataProvider().addFollower(player, target);
					ChatHandlerFunctions::showInfo(player, "Now following player " + target->getName());
				}
				else {
					ChatHandlerFunctions::showError(player, "Invalid player: " + playerName);
				}
			}
			else {
				ChatHandlerFunctions::showError(player, "You must specify a player to follow");
			}
		}
		return ChatResult::HandledDisplay;
	}

	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::changeChannel(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+))", matches) == MatchResult::AnyMatches) {
		string_t targetChannel = matches[1];
		channel_id_t channel = atoi(targetChannel.c_str()) - 1;
		player->changeChannel(channel);
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::lag(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+))", matches) == MatchResult::AnyMatches) {
		string_t target = matches[1];
		if (Player *p = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(target)) {
			ChatHandlerFunctions::showInfo(player, p->getName() + "'s lag: " + StringUtilities::lexical_cast<string_t>(p->getLatency().count()) + "ms");
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid player: " + target);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::header(Player *player, const chat_t &args) -> ChatResult {
	ChannelServer::getInstance().sendWorld(SyncPacket::ConfigPacket::scrollingHeader(args));
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::shutdown(Player *player, const chat_t &args) -> ChatResult {
	ChatHandlerFunctions::showInfo(player, "Shutting down the server");
	ChannelServer::getInstance().log(LogType::GmCommand, "GM shutdown the server. GM: " + player->getName());
	// TODO FIXME remove this or figure out a better way to post a shutdown than just doing the shutdown here
	ExitCodes::exit(ExitCodes::ForcedByGm);
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::kick(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(args)) {
			if (player->getGmLevel() > target->getGmLevel()) {
				target->disconnect();
				ChatHandlerFunctions::showInfo(player, "Kicked " + args + " from the server");
			}
			else {
				ChatHandlerFunctions::showError(player, "Player " + args + " is your peer or outranks you");
			}
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid player: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::relog(Player *player, const chat_t &args) -> ChatResult {
	player->changeChannel(ChannelServer::getInstance().getChannelId());
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::calculateRanks(Player *player, const chat_t &args) -> ChatResult {
	ChannelServer::getInstance().sendWorld(WorldServerConnectPacket::rankingCalculation());
	ChatHandlerFunctions::showInfo(player, "Sent a signal to force the calculation of rankings");
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::item(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(\d*)?)", matches) == MatchResult::AnyMatches) {
		string_t rawItem = matches[1];
		item_id_t itemId = atoi(rawItem.c_str());
		if (ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId) != nullptr) {
			string_t countString = matches[2];
			uint16_t count = countString.empty() ? 1 : atoi(countString.c_str());
			Inventory::addNewItem(player, itemId, count, Items::StatVariance::Gachapon);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid item: " + rawItem);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::storage(Player *player, const chat_t &args) -> ChatResult {
	NpcHandler::showStorage(player, 1012009);
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::shop(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		shop_id_t shopId = -1;
		if (args == "gear") shopId = 9999999;
		else if (args == "scrolls") shopId = 9999998;
		else if (args == "nx") shopId = 9999997;
		else if (args == "face") shopId = 9999996;
		else if (args == "ring") shopId = 9999995;
		else if (args == "chair") shopId = 9999994;
		else if (args == "mega") shopId = 9999993;
		else if (args == "pet") shopId = 9999992;
		else shopId = atoi(args.c_str());

		if (NpcHandler::showShop(player, shopId) == Result::Successful) {
			return ChatResult::HandledDisplay;
		}
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::reload(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		if (args == "items" || args == "drops" || args == "shops" ||
			args == "mobs" || args == "beauty" || args == "scripts" ||
			args == "skills" || args == "reactors" || args == "pets" ||
			args == "quests" || args == "all") {
			ChannelServer::getInstance().sendWorld(WorldServerConnectPacket::reloadMcdb(args));
			ChatHandlerFunctions::showInfo(player, "Reloading message for " + args + " sent to all channels");
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid reload type: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::npc(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+))", matches) == MatchResult::AnyMatches) {
		auto &provider = ChannelServer::getInstance().getNpcDataProvider();
		npc_id_t npcId = atoi(args.c_str());
		if (provider.isValidNpcId(npcId)) {
			Npc *npc = new Npc(npcId, player);
			npc->run();
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid NPC ID: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::addNpc(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+))", matches) == MatchResult::AnyMatches) {
		auto &provider = ChannelServer::getInstance().getNpcDataProvider();
		npc_id_t npcId = atoi(args.c_str());
		if (provider.isValidNpcId(npcId)) {
			NpcSpawnInfo npc;
			npc.id = npcId;
			npc.foothold = 0;
			npc.pos = player->getPos();
			npc.rx0 = npc.pos.x - 50;
			npc.rx1 = npc.pos.x + 50;
			map_object_t id = player->getMap()->addNpc(npc);
			ChatHandlerFunctions::showInfo(player, "Spawned NPC " + args + " with object ID " + StringUtilities::lexical_cast<string_t>(id));
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid NPC ID: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::killNpc(Player *player, const chat_t &args) -> ChatResult {
	player->setNpc(nullptr);
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::kill(Player *player, const chat_t &args) -> ChatResult {
	if (player->getGmLevel() == 1) {
		player->getStats()->setHp(0);
	}
	else {
		bool proceed = true;
		auto iterate = [&player](function_t<bool(Player *p)> func) -> int {
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
			ChatHandlerFunctions::showInfo(player, "Killed " + StringUtilities::lexical_cast<string_t>(kills) + " players in the current map");
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
			ChatHandlerFunctions::showInfo(player, "Killed " + StringUtilities::lexical_cast<string_t>(kills) + " " + (args == "gm" ? "GMs" : "players") + " in the current map");
		}
		if (proceed) {
			if (args == "me") {
				player->getStats()->setHp(0);
				ChatHandlerFunctions::showInfo(player, "Killed yourself");
			}
			else if (Player *kill = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(args)) {
				// Kill by name
				kill->getStats()->setHp(0);
				ChatHandlerFunctions::showInfo(player, "Killed " + args);
			}
			else {
				// Nothing valid
				return ChatResult::ShowSyntax;
			}
		}
	}
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::ban(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) ?(\d+)?)", matches) == MatchResult::AnyMatches) {
		string_t targetName = matches[1];
		if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(targetName)) {
			target->disconnect();
		}
		string_t reasonString = matches[2];
		int8_t reason = reasonString.empty() ? 1 : atoi(reasonString.c_str());

		// Ban account
		string_t expire{"2130-00-00 00:00:00"};

		auto &db = Database::getCharDb();
		auto &sql = db.getSession();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.makeTable("user_accounts") << " u "
			<< "INNER JOIN " << db.makeTable("characters") << " c ON u.user_id = c.user_id "
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
			string_t banMessage = targetName + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			ChannelServer::getInstance().getPlayerDataProvider().send(PlayerPacket::showMessage(banMessage, PlayerPacket::NoticeTypes::Notice));
			ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
				log << "GM " << player->getName()
					<< " banned a player with reason " << reason
					<< ", player: " << targetName;
			});
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid player: " + targetName);
		}

		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::tempBan(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (\d+) (\d+))", matches) == MatchResult::AnyMatches) {
		string_t targetName = matches[1];
		if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(targetName)) {
			target->disconnect();
		}
		string_t reasonString = matches[2];
		string_t length = matches[3];
		int8_t reason = reasonString.empty() ? 1 : atoi(reasonString.c_str());

		// Ban account
		auto &db = Database::getCharDb();
		auto &sql = db.getSession();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.makeTable("user_accounts") << " u "
			<< "INNER JOIN " << db.makeTable("characters") << " c ON u.user_id = c.user_id "
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
			string_t banMessage = targetName + " has been banned" + ChatHandlerFunctions::getBanString(reason);
			ChannelServer::getInstance().getPlayerDataProvider().send(PlayerPacket::showMessage(banMessage, PlayerPacket::NoticeTypes::Notice));

			ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
				log << "GM " << player->getName()
					<< " temporary banned a player with reason " << reason
					<< " for " << length
					<< " days, player: " << targetName;
			});
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid player: " + targetName);
		}

		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::ipBan(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) ?(\d+)?)", matches) == MatchResult::AnyMatches) {
		string_t targetName = matches[1];
		if (Player *target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(targetName)) {
			string_t targetIp = target->getIp().toString();
			target->disconnect();

			string_t reasonString = matches[2];
			int8_t reason = reasonString.empty() ? 1 : atoi(reasonString.c_str());

			// IP ban
			auto &db = Database::getCharDb();
			auto &sql = db.getSession();
			soci::statement st = (sql.prepare << "INSERT INTO " << db.makeTable("ip_bans") << " (ip) VALUES (:ip)", soci::use(targetIp, "ip"));

			st.execute();

			if (st.get_affected_rows() > 0) {
				string_t banMessage = targetName + " has been banned" + ChatHandlerFunctions::getBanString(reason);

				ChannelServer::getInstance().getPlayerDataProvider().send(PlayerPacket::showMessage(banMessage, PlayerPacket::NoticeTypes::Notice));
				ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
					log << "GM " << player->getName()
						<< " IP banned a player with reason " << reason
						<< ", player: " << targetName;
				});
			}
			else {
				ChatHandlerFunctions::showError(player, "Unknown error, couldn't ban " + targetIp);
			}
		}
		else {
			// TODO FIXME add raw IP banning
			ChatHandlerFunctions::showError(player, "Invalid player: " + targetName);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::unban(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		// Unban account
		auto &db = Database::getCharDb();
		auto &sql = db.getSession();
		soci::statement st = (sql.prepare
			<< "UPDATE " << db.makeTable("user_accounts") << " u "
			<< "INNER JOIN " << db.makeTable("characters") << " c ON u.user_id = c.user_id "
			<< "SET "
			<< "	u.banned = 0, "
			<< "	u.ban_reason = NULL, "
			<< "	u.ban_expire = NULL "
			<< "WHERE c.name = :name ",
			soci::use(args, "name"));

		st.execute();

		if (st.get_affected_rows() > 0) {
			string_t banMessage = args + " has been unbanned";
			ChatHandlerFunctions::showInfo(player, banMessage);
			ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
				log << "GM " << player->getName()
					<< " unbanned a player: " << args;
			});
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid player: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto ManagementFunctions::rehash(Player *player, const chat_t &args) -> ChatResult {
	ChannelServer::getInstance().sendWorld(WorldServerConnectPacket::rehashConfig());
	ChatHandlerFunctions::showInfo(player, "Sent a signal to force rehashing world configurations");
	return ChatResult::HandledDisplay;
}

auto ManagementFunctions::rates(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (!args.empty()) {
		if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) ?(\w+)? ?(\-?\d+)?)", matches) == MatchResult::NoMatches) {
			return ChatResult::ShowSyntax;
		}
		string_t type = matches[1];
		string_t classification = matches[2];
		if (!classification.empty()) {
			if (classification != "mobexp" &&
				classification != "questexp" &&
				classification != "drop" &&
				classification != "dropmeso" &&
				classification != "globaldrop" &&
				classification != "globaldropmeso") {
				return ChatResult::ShowSyntax;
			}
		}
		string_t value = matches[3];
		if (type == "view") {
			auto display = [player](const string_t &type, int32_t rate) {
				ChatHandlerFunctions::showInfo(player, type + " rate: " + StringUtilities::lexical_cast<string_t>(rate) + "x");
			};

			ChatHandlerFunctions::showInfo(player, "Current Rates");
			auto &config = ChannelServer::getInstance().getConfig();
			if (classification.empty() || classification == "mobexp") display("Mob EXP", config.rates.mobExpRate);
			if (classification.empty() || classification == "questexp") display("Quest EXP", config.rates.questExpRate);
			if (classification.empty() || classification == "drop") display("Drop", config.rates.dropRate);
			if (classification.empty() || classification == "dropmeso") display("Drop meso", config.rates.dropMeso);
			if (classification.empty() || classification == "globaldrop") {
				display("Global drop", config.rates.isGlobalDropConsistentWithRegularDropRate() ?
					config.rates.dropRate :
					config.rates.globalDropRate);
			}
			if (classification.empty() || classification == "globaldropmeso") {
				display("Global drop meso", config.rates.isGlobalDropMesoConsistentWithRegularDropMesoRate() ?
					config.rates.dropMeso :
					config.rates.globalDropMeso);
			}
		}
		else if (type == "reset") {
			if (classification.empty()) {
				ChatHandlerFunctions::showInfo(player, "Sent request to reset all rates");
				ChannelServer::getInstance().sendWorld(SyncPacket::ConfigPacket::resetRates(RatesConfig::Types::all));
			}
			else {
				int32_t rateType = 0;
				if (classification == "mobexp") rateType = RatesConfig::Types::mobExpRate;
				else if (classification == "drop") rateType = RatesConfig::Types::dropRate;
				else if (classification == "dropmeso") rateType = RatesConfig::Types::dropMeso;
				else if (classification == "questexp") rateType = RatesConfig::Types::questExpRate;
				else if (classification == "globaldrop") rateType = RatesConfig::Types::globalDropRate;
				else if (classification == "globaldropmeso") rateType = RatesConfig::Types::globalDropMeso;
				ChatHandlerFunctions::showInfo(player, "Sent request to reset specified rate");
				ChannelServer::getInstance().sendWorld(SyncPacket::ConfigPacket::resetRates(rateType));
			}
		}
		else if (type == "set") {
			if (classification.empty()) {
				return ChatResult::ShowSyntax;
			}

			int32_t rateType = 0;
			if (classification == "mobexp") rateType = RatesConfig::Types::mobExpRate;
			else if (classification == "drop") rateType = RatesConfig::Types::dropRate;
			else if (classification == "dropmeso") rateType = RatesConfig::Types::dropMeso;
			else if (classification == "questexp") rateType = RatesConfig::Types::questExpRate;
			else if (classification == "globaldrop") rateType = RatesConfig::Types::globalDropRate;
			else if (classification == "globaldropmeso") rateType = RatesConfig::Types::globalDropMeso;
			int32_t newAmount = value.empty() ?
				((rateType & RatesConfig::Types::global) != 0 ?
					RatesConfig::consistentRateBetweenGlobalAndRegular :
					1) :
				atoi(value.c_str());

			ChannelServer::getInstance().modifyRate(rateType, newAmount);
			ChatHandlerFunctions::showInfo(player, "Sent request to modify rate");
		}
		else {
			return ChatResult::ShowSyntax;
		}
	}
	else {
		return ChatResult::ShowSyntax;
	}
	return ChatResult::HandledDisplay;
}