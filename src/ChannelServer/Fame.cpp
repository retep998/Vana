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
#include "Fame.h"
#include "ChannelServer.h"
#include "Database.h"
#include "FamePacket.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"

auto Fame::handleFame(Player *player, PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	uint8_t type = packet.get<uint8_t>();
	if (player->getId() > 0) {
		if (player->getId() == playerId) {
			// Hacking
			return;
		}
		int32_t checkResult = canFame(player, playerId);
		if (checkResult != 0) {
			FamePacket::sendError(player, checkResult);
		}
		else {
			Player *famee = PlayerDataProvider::getInstance().getPlayer(playerId);
			int16_t newFame = famee->getStats()->getFame() + (type == 1 ? 1 : -1); // Increase if type = 1, else decrease
			famee->getStats()->setFame(newFame);
			addFameLog(player->getId(), playerId);
			FamePacket::sendFame(player, famee, type, newFame);
		}
	}
	else {
		FamePacket::sendError(player, FamePacket::Errors::IncorrectUser);
	}
}

auto Fame::canFame(Player *player, int32_t to) -> int32_t {
	int32_t from = player->getId();
	if (player->getStats()->getLevel() < 15) {
		return FamePacket::Errors::LevelUnder15;
	}
	if (getLastFameLog(from)) {
		return FamePacket::Errors::AlreadyFamedToday;
	}
	if (getLastFameSpLog(from, to)) {
		return FamePacket::Errors::FamedThisMonth;
	}
	return 0;
}

auto Fame::addFameLog(int32_t from, int32_t to) -> void {
	Database::getCharDb().once
		<< "INSERT INTO fame_log (from_character_id, to_character_id, fame_time) "
		<< "VALUES (:from, :to, NOW())",
		soci::use(from, "from"),
		soci::use(to, "to");
}

auto Fame::getLastFameLog(int32_t from) -> bool {
	int32_t fameTime = ChannelServer::getInstance().getFameTime();
	if (fameTime == 0) {
		return true;
	}
	if (fameTime == -1) {
		return false;
	}

	soci::session &sql = Database::getCharDb();
	opt_unix_time_t time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM fame_log "
		<< "WHERE from_character_id = :from AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fameTime "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(fameTime, "fameTime"),
		soci::into(time);

	return time.is_initialized();
}

auto Fame::getLastFameSpLog(int32_t from, int32_t to) -> bool {
	int32_t fameResetTime = ChannelServer::getInstance().getFameResetTime();
	if (fameResetTime == 0) {
		return true;
	}
	if (fameResetTime == -1) {
		return false;
	}

	soci::session &sql = Database::getCharDb();
	opt_unix_time_t time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM fame_log "
		<< "WHERE from_character_id = :from AND to_character_id = :to AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fameResetTime "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(to, "to"),
		soci::use(fameResetTime, "fameResetTime"),
		soci::into(time);

	return time.is_initialized();
}