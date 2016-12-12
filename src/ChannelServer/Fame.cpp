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
#include "Fame.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "FamePacket.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"

auto Fame::handleFame(Player *player, PacketReader &reader) -> void {
	player_id_t playerId = reader.get<player_id_t>();
	uint8_t type = reader.get<uint8_t>();
	if (player->getId() > 0) {
		if (player->getId() == playerId) {
			// Hacking
			return;
		}
		int32_t checkResult = canFame(player, playerId);
		if (checkResult != 0) {
			player->send(FamePacket::sendError(checkResult));
		}
		else {
			Player *famee = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
			fame_t newFame = famee->getStats()->getFame() + (type == 1 ? 1 : -1);
			famee->getStats()->setFame(newFame);
			addFameLog(player->getId(), playerId);
			player->send(FamePacket::sendFame(famee->getName(), type, newFame));
			famee->send(FamePacket::receiveFame(player->getName(), type));
		}
	}
	else {
		player->send(FamePacket::sendError(FamePacket::Errors::IncorrectUser));
	}
}

auto Fame::canFame(Player *player, player_id_t to) -> int32_t {
	player_id_t from = player->getId();
	if (player->getStats()->getLevel() < 15) {
		return FamePacket::Errors::LevelUnder15;
	}
	if (getLastFameLog(from) == SearchResult::Found) {
		return FamePacket::Errors::AlreadyFamedToday;
	}
	if (getLastFameSpLog(from, to) == SearchResult::Found) {
		return FamePacket::Errors::FamedThisMonth;
	}
	return 0;
}

auto Fame::addFameLog(player_id_t from, player_id_t to) -> void {
	Database::getCharDb().once
		<< "INSERT INTO " << Database::makeCharTable("fame_log") << " (from_character_id, to_character_id, fame_time) "
		<< "VALUES (:from, :to, NOW())",
		soci::use(from, "from"),
		soci::use(to, "to");
}

auto Fame::getLastFameLog(player_id_t from) -> SearchResult {
	int32_t fameTime = static_cast<int32_t>(ChannelServer::getInstance().getConfig().fameTime.count());
	if (fameTime == 0) {
		return SearchResult::Found;
	}
	if (fameTime == -1) {
		return SearchResult::NotFound;
	}

	soci::session &sql = Database::getCharDb();
	opt_unix_time_t time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM " << Database::makeCharTable("fame_log") << " "
		<< "WHERE from_character_id = :from AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fameTime "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(fameTime, "fameTime"),
		soci::into(time);

	return time.is_initialized() ? SearchResult::Found : SearchResult::NotFound;
}

auto Fame::getLastFameSpLog(player_id_t from, player_id_t to) -> SearchResult {
	int32_t fameResetTime = static_cast<int32_t>(ChannelServer::getInstance().getConfig().fameResetTime.count());
	if (fameResetTime == 0) {
		return SearchResult::Found;
	}
	if (fameResetTime == -1) {
		return SearchResult::NotFound;
	}

	soci::session &sql = Database::getCharDb();
	opt_unix_time_t time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM " << Database::makeCharTable("fame_log") << " "
		<< "WHERE from_character_id = :from AND to_character_id = :to AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fameResetTime "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(to, "to"),
		soci::use(fameResetTime, "fameResetTime"),
		soci::into(time);

	return time.is_initialized() ? SearchResult::Found : SearchResult::NotFound;
}