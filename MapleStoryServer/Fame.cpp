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
#include "Player.h"
#include "Players.h"
#include "MySQLM.h"
#include "PlayersPacket.h"
#include "BufferUtilities.h"

void Fame::handleFame(Player* player, unsigned char* packet){
	int CharID = getInt(packet);
	int FameDefame = 0;
	FameDefame = getInt(packet+4);
	int NewFame = 0;

	int checkResult = canFame(player,CharID);

	if(player->getPlayerid() > 0)
	{
		if(player->getPlayerid() != CharID)
		{
			if(checkResult >= 1 && checkResult <= 4)
			{
				FamePacket::SendError(player,checkResult);
			}
			else
			{
				if(FameDefame == 1)
				{
					NewFame = Players::players[CharID]->getFame()+1;
					Players::players[CharID]->setFame(NewFame);
					addFameLog(player->getPlayerid(), CharID);
					FamePacket::SendFame(player,Players::players[CharID],player->getName(),strlen(player->getName()),FameDefame,NewFame);
					FamePacket::UpdateFame(Players::players[CharID]);
					PlayersPacket::showInfo(player, Players::players[CharID]);
				}
				else if(FameDefame == 0)
				{
					NewFame = Players::players[CharID]->getFame()-1;
					Players::players[CharID]->setFame(NewFame);
					addFameLog(player->getPlayerid(), CharID);
					FamePacket::SendFame(player,Players::players[CharID],player->getName(),strlen(player->getName()),FameDefame,NewFame);
					FamePacket::UpdateFame(Players::players[CharID]);
					PlayersPacket::showInfo(player, Players::players[CharID]);
				}
			}
		}
		else
		{
			// Hacking
			return;
		}
	}
	else
	{
		FamePacket::SendError(player,1);
	}
}

int Fame::canFame(Player* player, int to){
	int from = player->getPlayerid();
	if (player->getLevel() < 15)
	{
		return 2;
	}
	else if(getLastFameLog(from))
	{
		return 3;
	}
	else if(getLastFameSPLog(from, to))
	{
		return 4;
	}
	else
	{
		return 0;
	}
}

void Fame::addFameLog(int from, int to){
	char query[255]; 
    sprintf_s(query, 255, "INSERT INTO `fame_log` (`from`,`to`,`time`) VALUES ('%d','%d', NOW());", from, to);
	MySQL::insert(query);
}

bool Fame::getLastFameLog(int from){ // Last fame from that char
	MYSQL_RES *mres;
	char query[255]; 
	sprintf_s(query, 255, "SELECT `time` FROM `fame_log` WHERE `from`='%d' AND UNIX_TIMESTAMP(`time`) > UNIX_TIMESTAMP()-86400 ORDER BY `time` DESC LIMIT 1;", from);
	
	if(mres = MySQL::getRes(query)){
		int rows = (int) mysql_num_rows(mres);
		mysql_free_result(mres);
		return (rows == 0) ? false : true;
	} else {
		return false;
	}
}

bool Fame::getLastFameSPLog(int from, int to){
	MYSQL_RES *mres;
	char query[255]; 
    sprintf_s(query, 255, "SELECT `time` FROM `fame_log` WHERE `from`='%d' AND `to`='%d' AND UNIX_TIMESTAMP(`time`) > UNIX_TIMESTAMP()-2592000 ORDER BY `time` DESC LIMIT 1;", from, to);
	if(mres = MySQL::getRes(query)){
		int rows = (int) mysql_num_rows(mres);
		mysql_free_result(mres);
		return (rows == 0) ? false : true;
	} else {
		return false;
	}
}