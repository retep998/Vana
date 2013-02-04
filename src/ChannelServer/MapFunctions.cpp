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
#include "MapFunctions.h"
#include "ChannelServer.h"
#include "Map.h"
#include "MapPacket.h"
#include "MobConstants.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "StringUtilities.h"
#include <chrono>
#include <iostream>

using StringUtilities::lexical_cast;

bool MapFunctions::eventInstruction(Player *player, const string &args) {
	MapPacket::showEventInstructions(player->getMapId());
	return true;
}

bool MapFunctions::instruction(Player *player, const string &args) {
	if (args.length() != 0) {
		Map *map = player->getMap();
		for (size_t i = 0; i < map->getNumPlayers(); i++) {
			PlayerPacket::instructionBubble(map->getPlayer(i), args);
		}
		PlayerPacket::showMessage(player, "Showing instruction bubble to everyone on the map.", PlayerPacket::NoticeTypes::Blue);
		return true;
	}
	return false;
}

bool MapFunctions::timer(Player *player, const string &args) {
	if (args.length() != 0) {
		seconds_t time(atoi(args.c_str()));
		std::ostringstream msg;
		msg << "Stopped map timer.";
		if (time.count() > 0) {
			msg.str("");
			msg.clear();
			msg << "Started map timer. Counting down from ";

			hours_t hours = std::chrono::duration_cast<hours_t>(time);
			minutes_t minutes = std::chrono::duration_cast<minutes_t>(time - hours);
			seconds_t seconds = std::chrono::duration_cast<seconds_t>(time - hours - minutes);

			if (hours.count() > 0) {
				msg << hours.count() << " hours";
			}
			if (minutes.count() > 0) {
				if (hours.count() > 0) {
					msg << ", ";
				}
				msg << minutes.count() << " minutes";
			}
			if (seconds.count() > 0) {
				if (hours.count() > 0 || minutes.count() > 0) {
					msg << " and ";
				}
				msg << seconds.count() << " seconds";
			}
			msg << "!";
		}
		PlayerPacket::showMessage(player, msg.str(), PlayerPacket::NoticeTypes::Blue);
		player->getMap()->setMapTimer(time);
		return true;
	}
	return false;
}

bool MapFunctions::killMob(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t mobId = atoi(args.c_str());
		Mob *mob = player->getMap()->getMob(mobId);
		if (mob != nullptr) {
			PlayerPacket::showMessage(player, "Killed mob with map mob ID " + args + ". Damage applied: " + lexical_cast<string>(mob->getMaxHp() - mob->getHp()) + ".", PlayerPacket::NoticeTypes::Blue);
			mob->applyDamage(player->getId(), mob->getHp());
		}
		else {
			PlayerPacket::showMessage(player, "Couldn't kill mob. Mob with map mob ID " + args + " not found.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool MapFunctions::getMobHp(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t mobId = atoi(args.c_str());
		Mob *mob = player->getMap()->getMob(mobId);
		if (mob != nullptr) {
			std::ostringstream message;
			message << "Mob " << mobId
					<< " HP: " << mob->getHp() << "/" << mob->getMaxHp()
					<< " (" << static_cast<int64_t>(mob->getHp()) * 100 / mob->getMaxHp() << "%)";

			PlayerPacket::showMessage(player, message.str(), PlayerPacket::NoticeTypes::Blue);
		}
		else {
			PlayerPacket::showMessage(player, "Mob does not exist.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool MapFunctions::listMobs(Player *player, const string &args) {
	if (player->getMap()->countMobs(0) > 0) {
		typedef unordered_map<int32_t, Mob *> MobMap;
		MobMap mobs = player->getMap()->getMobs();
		std::ostringstream message;
		for (MobMap::iterator iter = mobs.begin(); iter != mobs.end(); ++iter) {
			message.str("");
			message.clear();

			message << "Mob " << iter->first
					<< " (ID: " << iter->second->getMobId()
					<< ", HP: " << iter->second->getHp()
					<< "/" << iter->second->getMaxHp()
					<< " [" << static_cast<int64_t>(iter->second->getHp()) * 100 / iter->second->getMaxHp()
					<< "%])";

			PlayerPacket::showMessage(player, message.str(), PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		PlayerPacket::showMessage(player, "No mobs on the current map.", PlayerPacket::NoticeTypes::Red);
	}
	return true;
}

bool MapFunctions::zakum(Player *player, const string &args) {
	player->getMap()->spawnZakum(player->getPos());
	ChannelServer::Instance()->log(LogTypes::GmCommand, "Player spawned Zakum on map " + lexical_cast<string>(player->getMap()) + ". Name: " + player->getName());
	return true;
}

bool MapFunctions::horntail(Player *player, const string &args) {
	player->getMap()->spawnMob(Mobs::SummonHorntail, player->getPos());
	ChannelServer::Instance()->log(LogTypes::GmCommand, "Player spawned Horntail on map " + lexical_cast<string>(player->getMap()) + ". Name: " + player->getName());
	return true;
}

bool MapFunctions::music(Player *player, const string &args) {
	soci::session &sql = Database::getDataDb();
	string music;

	sql
		<< "SELECT m.default_bgm "
		<< "FROM map_data m "
		<< "WHERE m.default_bgm = :q "
		<< "LIMIT 1",
		soci::use(args, "q"),
		soci::into(music);

	if (music.empty()) {
		PlayerPacket::showMessage(player, "Invalid music: " + args, PlayerPacket::NoticeTypes::Red);
	}
	else {
		player->getMap()->setMusic(music);
		PlayerPacket::showMessage(player, "Set music on the map to: " + music, PlayerPacket::NoticeTypes::Blue);
	}
	return true;
}

bool MapFunctions::summon(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\d+) ?(\\d+)?", matches)) {
		int32_t mobId = atoi(string(matches[1]).c_str());
		if (MobDataProvider::Instance()->mobExists(mobId)) {
			string countString = matches[2];
			int32_t count = countString.length() > 0 ? atoi(countString.c_str()) : 1;
			if (count > 100) count = 100;
			for (int32_t i = 0; i < count; i++) {
				player->getMap()->spawnMob(mobId, player->getPos());
			}
			if (count > 0) {
				PlayerPacket::showMessage(player, "Spawned " + lexical_cast<string>(count) + " mobs with ID " + lexical_cast<string>(mobId) + ".", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				PlayerPacket::showMessage(player, "No mobs spawned.", PlayerPacket::NoticeTypes::Red);
			}
		}
		else {
			PlayerPacket::showMessage(player, "Invalid Mob ID.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool MapFunctions::clearDrops(Player *player, const string &args) {
	player->getMap()->clearDrops();
	return true;
}

bool MapFunctions::killAllMobs(Player *player, const string &args) {
	int32_t killed = player->getMap()->killMobs(player);
	PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(killed) + " mobs!", PlayerPacket::NoticeTypes::Blue);
	return true;
}