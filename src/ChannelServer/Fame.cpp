/*
Copyright (C) 2008 Vana Development Team

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
#include "FamePacket.h"
#include "PlayerPacket.h"
#include "Player.h"
#include "Players.h"
#include "MySQLM.h"
#include "PlayersPacket.h"
#include "ReadPacket.h"

void Fame::handleFame(Player *player, ReadPacket *packet) {
	int playerid = packet->getInt();
	int type = packet->getInt();
	if (player->getId() > 0) {
		if (player->getId() != playerid) {
			int checkResult = canFame(player, playerid);
			if (checkResult >= 1 && checkResult <= 4)
				FamePacket::sendError(player, checkResult);
			else {
				Player *famee = Players::Instance()->getPlayer(playerid);
				int newFame = 0;
				if (type == 1)
					newFame = famee->getFame() + 1;
				else if (type == 0)
					newFame = famee->getFame() - 1;
				famee->setFame(newFame);
				addFameLog(player->getId(), playerid);
				FamePacket::sendFame(player, famee, type, newFame);
			}
		}
		else {
			// Hacking
			return;
		}
	}
	else
		FamePacket::sendError(player, 1);
}

int Fame::canFame(Player *player, int to) {
	int from = player->getId();
	if (player->getLevel() < 15)
		return 2;
	if (getLastFameLog(from))
		return 3;
	if (getLastFameSPLog(from, to))
		return 4;
	return 0;
}

void Fame::addFameLog(int from, int to) {
	mysqlpp::Query query = Database::chardb.query();
	query << "INSERT INTO fame_log (`from`, `to`, `time`) VALUES (" 
			<< mysqlpp::quote << from << ","
			<< mysqlpp::quote << to << ", NOW())";
	query.exec();
}

bool Fame::getLastFameLog(int from) { // Last fame from that char
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT `time` FROM `fame_log` WHERE `from`=" << mysqlpp::quote << from << " AND UNIX_TIMESTAMP(`time`) > UNIX_TIMESTAMP()-86400 ORDER BY `time` DESC LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();
	if (!res.empty())
		return (res.num_rows() == 0) ? false : true;
	return false;
}

bool Fame::getLastFameSPLog(int from, int to) {
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT `time` FROM `fame_log` WHERE `from`=" << mysqlpp::quote << from << " AND `to`=" << mysqlpp::quote << to << " AND UNIX_TIMESTAMP(`time`) > UNIX_TIMESTAMP()-2592000 ORDER BY `time` DESC LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();
	if (!res.empty())
		return (res.num_rows() == 0) ? false : true;
	return false;
}
