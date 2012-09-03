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
#include "MapFunctions.h"
#include "ChannelServer.h"
#include "MapPacket.h"
#include "Maps.h"
#include "MobConstants.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "StringUtilities.h"
#include <iostream>

using StringUtilities::lexical_cast;

bool MapFunctions::eventInstruction(Player *player, const string &args) {
	MapPacket::showEventInstructions(player->getMap());
	return true;
}

bool MapFunctions::instruction(Player *player, const string &args) {
	if (args.length() != 0) {
		for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
			PlayerPacket::instructionBubble(Maps::getMap(player->getMap())->getPlayer(i), args);
		}
		PlayerPacket::showMessage(player, "Showing instruction bubble to everyone on the map.", PlayerPacket::NoticeTypes::Blue);
		return true;
	}
	return false;
}

bool MapFunctions::timer(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t time = atoi(args.c_str());
		std::ostringstream msg;
		msg << "Stopped map timer.";
		if (time > 0) {
			int32_t seconds = time /= 60;
			int32_t minutes = time /= 60;
			int32_t hours = time /= 60;
			msg.str("");
			msg.clear();
			msg << "Started map timer. Counting down from ";

			if (hours > 0) {
				msg << hours << " hours";
			}
			if (minutes > 0) {
				if (hours > 0) {
					msg << ", ";
				}
				msg << minutes << " minutes";
			}
			if (seconds > 0) {
				if (hours > 0 || minutes > 0) {
					msg << " and ";
				}
				msg << seconds << " seconds";
			}
			msg << "!";
		}
		PlayerPacket::showMessage(player, msg.str(), PlayerPacket::NoticeTypes::Blue);
		Maps::getMap(player->getMap())->setMapTimer(time);
		return true;
	}
	return false;
}

bool MapFunctions::killMob(Player *player, const string &args) {
	if (args.length() != 0) {
		int32_t mobId = atoi(args.c_str());
		Mob *mob = Maps::getMap(player->getMap())->getMob(mobId);
		if (mob != nullptr) {
			PlayerPacket::showMessage(player, "Killed mob with map mob ID " + args + ". HP left: " + lexical_cast<string>(mob->getMaxHp() - mob->getHp()) + ".", PlayerPacket::NoticeTypes::Blue);
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
		Mob *mob = Maps::getMap(player->getMap())->getMob(mobId);
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
	if (Maps::getMap(player->getMap())->countMobs(0) > 0) {
		typedef unordered_map<int32_t, Mob *> MobMap;
		MobMap mobs = Maps::getMap(player->getMap())->getMobs();
		std::ostringstream message;
		for (MobMap::iterator iter = mobs.begin(); iter != mobs.end(); ++iter) {
			message.str("");
			message.clear();

			message << "Mob " << iter->first
					<< " (ID: " << iter->second->getMobId()
					<< ", HP: " << iter->second->getHp()
					<< "/" << iter->second->getMaxHp()
					<< ")";

			PlayerPacket::showMessage(player, message.str(), PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		PlayerPacket::showMessage(player, "No mobs on the current map.", PlayerPacket::NoticeTypes::Red);
	}
	return true;
}

bool MapFunctions::zakum(Player *player, const string &args) {
	Maps::getMap(player->getMap())->spawnZakum(player->getPos());
	ChannelServer::Instance()->log(LogTypes::GmCommand, "Player spawned Zakum on map " + lexical_cast<string>(player->getMap()) + ". Name: " + player->getName());
	return true;
}

bool MapFunctions::horntail(Player *player, const string &args) {
	Maps::getMap(player->getMap())->spawnMob(Mobs::SummonHorntail, player->getPos());
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
		Maps::getMap(player->getMap())->setMusic(music);
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
				Maps::getMap(player->getMap())->spawnMob(mobId, player->getPos());
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
	Maps::getMap(player->getMap())->clearDrops();
	return true;
}

bool MapFunctions::killAllMobs(Player *player, const string &args) {
	int32_t killed = Maps::getMap(player->getMap())->killMobs(player);
	PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(killed) + " mobs!", PlayerPacket::NoticeTypes::Blue);
	return true;
}