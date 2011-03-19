/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "WarpFunctions.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

namespace Functors {
	struct WarpFunctor {
		void operator() (Player *warpee) {
			if (warpee->getMap() != mapid) {
				warpee->setMap(mapid);
			}
		}
		int32_t mapid;
		Player *player;
	};
}

bool WarpFunctions::warpAll(Player *player, const string &args) {
	int32_t mapid = args.length() != 0 ? ChatHandlerFunctions::getMap(args, player) : player->getMap();

	if (Maps::getMap(mapid)) {
		Functors::WarpFunctor func = {mapid, player};
		PlayerDataProvider::Instance()->run(func);
		if (args.length() > 0) {
			PlayerPacket::showMessage(player, "Warped everyone in the server to map ID " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			PlayerPacket::showMessage(player, "Warped everyone in the server to yourself.", PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		PlayerPacket::showMessage(player, "Cannot warp players; invalid Map ID given (map not found).", PlayerPacket::NoticeTypes::Red);
	}
	return true;
}

bool WarpFunctions::warp(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) ?(\\d*)?", matches)) {
		string targetname = matches[1];
		if (Player *warpee = PlayerDataProvider::Instance()->getPlayer(targetname)) {
			string mapstring = matches[2];
			int32_t mapid = mapstring.length() > 0 ? ChatHandlerFunctions::getMap(mapstring, player) : player->getMap();

			if (Maps::getMap(mapid)) {
				warpee->setMap(mapid);
				PlayerPacket::showMessage(player, "Warped " + targetname + " to map ID " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				PlayerPacket::showMessage(player, "Cannot warp player; invalid map ID given.", PlayerPacket::NoticeTypes::Red);
			}
		}
		else {
			PlayerPacket::showMessage(player, targetname + " not found for warping.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool WarpFunctions::warpMap(Player *player, const string &args) {
	int32_t mapid = args.length() != 0 ? ChatHandlerFunctions::getMap(args, player) : player->getMap();

	if (Map *map = Maps::getMap(mapid)) {
		Functors::WarpFunctor func = {mapid, player};
		Maps::getMap(player->getMap())->runFunctionPlayers(func);
		if (args.length() > 0) {
			PlayerPacket::showMessage(player, "Warped everyone in the map to map ID " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			PlayerPacket::showMessage(player, "Warped everyone in the map to yourself.", PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		PlayerPacket::showMessage(player, "Cannot warp players; invalid Map ID given (map not found).", PlayerPacket::NoticeTypes::Red);
	}
	return true;
}

bool WarpFunctions::warpTo(Player *player, const string &args) {
	if (args.length() > 0) {
		if (Player *warptoee = PlayerDataProvider::Instance()->getPlayer(args)) {
			player->setMap(warptoee->getMap());
		}
		else {
			PlayerPacket::showMessage(player, "Player not found: " + args, PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}