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
#include "MapFunctions.hpp"
#include "Algorithm.hpp"
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

auto MapFunctions::eventInstruction(Player *player, const chat_t &args) -> ChatResult {
	player->sendMap(MapPacket::showEventInstructions());
	return ChatResult::HandledDisplay;
}

auto MapFunctions::instruction(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->sendMap(PlayerPacket::instructionBubble(args));
		ChatHandlerFunctions::showInfo(player, "Showing instruction bubble to everyone on the map");
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MapFunctions::timer(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
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
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MapFunctions::killMob(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		map_object_t mobId = atoi(args.c_str());
		auto mob = player->getMap()->getMob(mobId);
		if (mob != nullptr) {
			ChatHandlerFunctions::showInfo(player, "Killed mob with map mob ID " + args + ". Damage applied: " + StringUtilities::lexical_cast<string_t>(mob->getHp()));
			mob->applyDamage(player->getId(), mob->getHp());
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid mob: " + args);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MapFunctions::getMobHp(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		map_object_t mobId = atoi(args.c_str());
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
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MapFunctions::listMobs(Player *player, const chat_t &args) -> ChatResult {
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
	return ChatResult::HandledDisplay;
}

auto MapFunctions::listPortals(Player *player, const chat_t &args) -> ChatResult {
	map_id_t mapId = args.empty() ? player->getMapId() : ChatHandlerFunctions::getMap(args, player);
	Map *map = Maps::getMap(mapId);
	if (map == nullptr) {
		ChatHandlerFunctions::showError(player, "Invalid map: " + args);
		return ChatResult::HandledDisplay;
	}

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT mp.id, mp.label, mp.destination, mp.destination_label, mp.script "
		<< "FROM " << Database::makeDataTable("map_portals") << " mp "
		<< "WHERE mp.mapid = :mapId",
		soci::use(mapId, "mapId"));

	auto format = [](const soci::row &row, out_stream_t &str) {
		map_id_t destination = row.get<map_id_t>(2);
		opt_string_t destinationLabel = row.get<opt_string_t>(3);
		opt_string_t portalScript = row.get<opt_string_t>(4);

		str << row.get<map_id_t>(0) << " : " << row.get<string_t>(1);
		if (destination != Maps::NoMap) {
			str << " (destination " << destination;
			if (destinationLabel.is_initialized()) {
				string_t label = destinationLabel.get();
				if (!label.empty()) {
					str << " -> " << destinationLabel.get();
				}
			}
			str << ")";
		}

		if (portalScript.is_initialized()) {
			string_t script = portalScript.get();
			if (!script.empty()) {
				str << " (script '" << script << "')";
			}
		}
	};

	ChatHandlerFunctions::showInfo(player, "Portals for Map " + std::to_string(mapId));

	out_stream_t str("");
	bool found = false;
	for (const auto &row : rs) {
		string_t portalLabel = row.get<string_t>(1);
		if (portalLabel == "sp" || portalLabel == "tp") {
			continue;
		}
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		ChatHandlerFunctions::showInfo(player, str.str());
	}

	if (!found) {
		ChatHandlerFunctions::showError(player, "No results");
	}

	return ChatResult::HandledDisplay;
}

auto MapFunctions::listReactors(Player *player, const chat_t &args) -> ChatResult {
	map_id_t mapId = args.empty() ? player->getMapId() : ChatHandlerFunctions::getMap(args, player);
	Map *map = Maps::getMap(mapId);
	if (map == nullptr) {
		ChatHandlerFunctions::showError(player, "Invalid map: " + args);
		return ChatResult::HandledDisplay;
	}

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT ml.lifeid, sc.script "
		<< "FROM " << Database::makeDataTable("map_life") << " ml "
		<< "LEFT OUTER JOIN " << Database::makeDataTable("scripts") << " sc ON sc.objectid = ml.lifeid AND sc.script_type = 'reactor' "
		<< "WHERE ml.life_type = 'reactor' AND ml.mapid = :mapId",
		soci::use(mapId, "mapId"));

	auto format = [](const soci::row &row, out_stream_t &str) {
		opt_string_t reactorScript = row.get<opt_string_t>(1);

		str << row.get<reactor_id_t>(0);
		if (reactorScript.is_initialized()) {
			string_t script = reactorScript.get();
			if (!script.empty()) {
				str << " (script '" << script << "')";
			}
		}
	};

	ChatHandlerFunctions::showInfo(player, "Reactors for Map " + std::to_string(mapId));

	out_stream_t str("");
	bool found = false;
	for (const auto &row : rs) {
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		ChatHandlerFunctions::showInfo(player, str.str());
	}

	if (!found) {
		ChatHandlerFunctions::showError(player, "No results");
	}

	return ChatResult::HandledDisplay;
}

auto MapFunctions::listNpcs(Player *player, const chat_t &args) -> ChatResult {
	map_id_t mapId = args.empty() ? player->getMapId() : ChatHandlerFunctions::getMap(args, player);
	Map *map = Maps::getMap(mapId);
	if (map == nullptr) {
		ChatHandlerFunctions::showError(player, "Invalid map: " + args);
		return ChatResult::HandledDisplay;
	}

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT ml.lifeid, st.label, sc.script "
		<< "FROM " << Database::makeDataTable("map_life") << " ml "
		<< "INNER JOIN " << Database::makeDataTable("strings") << " st ON st.objectid = ml.lifeid AND st.object_type = 'npc' "
		<< "LEFT OUTER JOIN " << Database::makeDataTable("scripts") << " sc ON sc.objectid = ml.lifeid AND sc.script_type = 'npc' "
		<< "WHERE ml.life_type = 'npc' AND ml.mapid = :mapId",
		soci::use(mapId, "mapId"));

	auto format = [](const soci::row &row, out_stream_t &str) {
		str << row.get<npc_id_t>(0) << " : " << row.get<string_t>(1);
		opt_string_t script = row.get<opt_string_t>(2);
		if (script.is_initialized()) {
			str << " (script '" << script.get() << "')";
		}
	};

	ChatHandlerFunctions::showInfo(player, "NPCs for Map " + std::to_string(mapId));

	out_stream_t str("");
	bool found = false;
	for (const auto &row : rs) {
		found = true;

		str.str("");
		str.clear();
		format(row, str);
		ChatHandlerFunctions::showInfo(player, str.str());
	}

	if (!found) {
		ChatHandlerFunctions::showError(player, "No results");
	}

	return ChatResult::HandledDisplay;
}

auto MapFunctions::mapDimensions(Player *player, const chat_t &args) -> ChatResult {
	map_id_t mapId = args.empty() ? player->getMapId() : ChatHandlerFunctions::getMap(args, player);
	Map *map = Maps::getMap(mapId);
	if (map == nullptr) {
		ChatHandlerFunctions::showError(player, "Invalid map: " + args);
	}
	else {
		ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) {
			message << "Dimensions for Map " << mapId << ": " << map->getDimensions();
		});
	}
	return ChatResult::HandledDisplay;
}

auto MapFunctions::zakum(Player *player, const chat_t &args) -> ChatResult {
	player->getMap()->spawnZakum(player->getPos());
	ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
		log << "GM " << player->getName()
			<< " spawned Zakum on map " << player->getMapId();
	});
	return ChatResult::HandledDisplay;
}

auto MapFunctions::horntail(Player *player, const chat_t &args) -> ChatResult {
	player->getMap()->spawnMob(Mobs::SummonHorntail, player->getPos());
	ChannelServer::getInstance().log(LogType::GmCommand, [&](out_stream_t &log) {
		log << "GM " << player->getName()
			<< " spawned Horntail on map " << player->getMapId();
	});
	return ChatResult::HandledDisplay;
}

auto MapFunctions::music(Player *player, const chat_t &args) -> ChatResult {
	if (args.empty()) {
		ChatHandlerFunctions::showInfo(player, "Current music: " + player->getMap()->getMusic());
	}
	else {
		soci::session &sql = Database::getDataDb();
		string_t music;

		if (args == "default") {
			music = args;
		}
		else {
			sql
				<< "SELECT m.default_bgm "
				<< "FROM " << Database::makeDataTable("map_data") << " m "
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
	}
	return ChatResult::HandledDisplay;
}

auto MapFunctions::summon(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(\d+)?)", matches) == MatchResult::AnyMatches) {
		string_t rawMobId = matches[1];
		mob_id_t mobId = atoi(rawMobId.c_str());
		if (ChannelServer::getInstance().getMobDataProvider().mobExists(mobId)) {
			string_t countString = matches[2];
			int32_t count = ext::constrain_range(countString.empty() ? 1 : atoi(countString.c_str()), 1, 1000);
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
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MapFunctions::clearDrops(Player *player, const chat_t &args) -> ChatResult {
	player->getMap()->clearDrops();
	return ChatResult::HandledDisplay;
}

auto MapFunctions::killAllMobs(Player *player, const chat_t &args) -> ChatResult {
	int32_t killed = player->getMap()->killMobs(player, true);
	ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) { message << "Killed " << killed << " mobs"; });
	return ChatResult::HandledDisplay;
}