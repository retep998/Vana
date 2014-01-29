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
#include "MapFunctions.hpp"
#include "ChannelServer.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "MapPacket.hpp"
#include "MobConstants.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "StringUtilities.hpp"
#include <chrono>
#include <iostream>

auto MapFunctions::eventInstruction(Player *player, const string_t &args) -> bool {
	MapPacket::showEventInstructions(player->getMapId());
	return true;
}

auto MapFunctions::instruction(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		Map *map = player->getMap();
		for (size_t i = 0; i < map->getNumPlayers(); i++) {
			PlayerPacket::instructionBubble(map->getPlayer(i), args);
		}
		ChatHandlerFunctions::showInfo(player, "Showing instruction bubble to everyone on the map");
		return true;
	}
	return false;
}

auto MapFunctions::timer(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		seconds_t time(atoi(args.c_str()));
		out_stream_t msg;
		msg << "Stopped map timer";
		if (time.count() > 0) {
			msg.str("");
			msg.clear();
			msg << "Started map timer. Counting down from ";

			hours_t hours = duration_cast<hours_t>(time);
			minutes_t minutes = duration_cast<minutes_t>(time - hours);
			seconds_t seconds = duration_cast<seconds_t>(time - hours - minutes);

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
		}
		ChatHandlerFunctions::showInfo(player, msg.str());
		player->getMap()->setMapTimer(time);
		return true;
	}
	return false;
}

auto MapFunctions::killMob(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		int32_t mobId = atoi(args.c_str());
		auto mob = player->getMap()->getMob(mobId);
		if (mob != nullptr) {
			ChatHandlerFunctions::showInfo(player, "Killed mob with map mob ID " + args + ". Damage applied: " + StringUtilities::lexical_cast<string_t>(mob->getHp()));
			mob->applyDamage(player->getId(), mob->getHp());
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid mob: " + args);
		}
		return true;
	}
	return false;
}

auto MapFunctions::getMobHp(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		int32_t mobId = atoi(args.c_str());
		auto mob = player->getMap()->getMob(mobId);
		if (mob != nullptr) {
			out_stream_t message;
			message << "Mob " << mobId
					<< " HP: " << mob->getHp() << "/" << mob->getMaxHp()
					<< " (" << static_cast<int64_t>(mob->getHp()) * 100 / mob->getMaxHp() << "%)";

			ChatHandlerFunctions::showInfo(player, message.str());
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid mob: " + args);
		}
		return true;
	}
	return false;
}

auto MapFunctions::listMobs(Player *player, const string_t &args) -> bool {
	if (player->getMap()->countMobs(0) > 0) {
		out_stream_t message;
		player->getMap()->runFunctionMobs([&message, &player](ref_ptr_t<const Mob> mob) {
			message.str("");
			message.clear();

			message << "Mob " << mob->getMapMobId()
				<< " (ID: " << mob->getMobId()
				<< ", HP: " << mob->getHp()
				<< "/" << mob->getMaxHp()
				<< " [" << static_cast<int64_t>(mob->getHp()) * 100 / mob->getMaxHp()
				<< "%])";

			ChatHandlerFunctions::showInfo(player, message.str());
		});
	}
	else {
		ChatHandlerFunctions::showError(player, "No mobs exist on the current map");
	}
	return true;
}

auto MapFunctions::listPortals(Player *player, const string_t &args) -> bool {
	int32_t mapId = args.length() == 0 ? player->getMapId() : ChatHandlerFunctions::getMap(args, player);
	Map *map = Maps::getMap(mapId);
	if (map == nullptr) {
		ChatHandlerFunctions::showError(player, "Invalid map: " + args);
		return true;
	}

	auto portals = map->getPortalNames();
	string_t rawMapId = StringUtilities::lexical_cast<string_t>(mapId);
	if (portals.size() == 0) {
		ChatHandlerFunctions::showError(player, "Map " + rawMapId + " has no portals");
		return true;
	}

	ChatHandlerFunctions::showInfo(player, "Portals on map " + std::to_string(mapId) + ": " + StringUtilities::delimit(", ", portals));

	return true;
}

auto MapFunctions::zakum(Player *player, const string_t &args) -> bool {
	player->getMap()->spawnZakum(player->getPos());
	ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
		log << "GM " << player->getName()
			<< " spawned Zakum on map " << player->getMapId();
	});
	return true;
}

auto MapFunctions::horntail(Player *player, const string_t &args) -> bool {
	player->getMap()->spawnMob(Mobs::SummonHorntail, player->getPos());
	ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
		log << "GM " << player->getName()
			<< " spawned Horntail on map " << player->getMapId();
	});
	return true;
}

auto MapFunctions::music(Player *player, const string_t &args) -> bool {
	soci::session &sql = Database::getDataDb();
	string_t music;

	if (args == "default") {
		music = args;
	}
	else {
		sql
			<< "SELECT m.default_bgm "
			<< "FROM map_data m "
			<< "WHERE m.default_bgm = :q "
			<< "LIMIT 1",
			soci::use(args, "q"),
			soci::into(music);
	}

	if (music.empty()) {
		ChatHandlerFunctions::showError(player, "Invalid music: " + args);
	}
	else {
		player->getMap()->setMusic(music);
		ChatHandlerFunctions::showInfo(player, "Set music on the map to: " + music);
	}
	return true;
}

auto MapFunctions::summon(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(\d+)?)", matches)) {
		string_t rawMobId = matches[1];
		int32_t mobId = atoi(rawMobId.c_str());
		if (MobDataProvider::getInstance().mobExists(mobId)) {
			string_t countString = matches[2];
			int32_t count = std::max(countString.length() > 0 ? atoi(countString.c_str()) : 1, 100);
			for (int32_t i = 0; i < count; ++i) {
				player->getMap()->spawnMob(mobId, player->getPos());
			}
			if (count > 0) {
				ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) {
					message << "Spawned " << count
						<< " mobs with ID " << mobId;
				});
			}
			else {
				ChatHandlerFunctions::showError(player, "No mobs spawned");
			}
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid mob: " + rawMobId);
		}
		return true;
	}
	return false;
}

auto MapFunctions::clearDrops(Player *player, const string_t &args) -> bool {
	player->getMap()->clearDrops();
	return true;
}

auto MapFunctions::killAllMobs(Player *player, const string_t &args) -> bool {
	int32_t killed = player->getMap()->killMobs(player);
	ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) { message << "Killed " << killed << " mobs"; });
	return true;
}