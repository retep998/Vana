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
#include "WarpFunctions.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include "StringUtilities.h"

using StringUtilities::lexical_cast;

namespace Functors {
	struct WarpFunctor {
		void operator() (Player *warpee) {
			if (warpee->getMapId() != mapId) {
				warpee->setMap(mapId);
			}
		}
		int32_t mapId;
		Player *player;
	};
}

bool WarpFunctions::warpAll(Player *player, const string &args) {
	int32_t mapId = args.length() != 0 ? ChatHandlerFunctions::getMap(args, player) : player->getMapId();

	if (Maps::getMap(mapId)) {
		Functors::WarpFunctor func = {mapId, player};
		PlayerDataProvider::Instance()->run(func);
		if (args.length() > 0) {
			PlayerPacket::showMessage(player, "Warped everyone in the server to map ID " + lexical_cast<string>(mapId) + ".", PlayerPacket::NoticeTypes::Blue);
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
		string targetName = matches[1];
		if (Player *warpee = PlayerDataProvider::Instance()->getPlayer(targetName)) {
			string mapString = matches[2];
			int32_t mapId = mapString.length() > 0 ? ChatHandlerFunctions::getMap(mapString, player) : player->getMapId();

			if (Maps::getMap(mapId)) {
				warpee->setMap(mapId);
				PlayerPacket::showMessage(player, "Warped " + targetName + " to map ID " + lexical_cast<string>(mapId) + ".", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				PlayerPacket::showMessage(player, "Cannot warp player; invalid map ID given.", PlayerPacket::NoticeTypes::Red);
			}
		}
		else {
			PlayerPacket::showMessage(player, targetName + " not found for warping.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool WarpFunctions::warpMap(Player *player, const string &args) {
	int32_t mapId = args.length() != 0 ? ChatHandlerFunctions::getMap(args, player) : player->getMapId();

	if (Map *map = Maps::getMap(mapId)) {
		Functors::WarpFunctor func = {mapId, player};
		player->getMap()->runFunctionPlayers(func);
		if (args.length() > 0) {
			PlayerPacket::showMessage(player, "Warped everyone in the map to map ID " + lexical_cast<string>(mapId) + ".", PlayerPacket::NoticeTypes::Blue);
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
		if (Player *warpTo = PlayerDataProvider::Instance()->getPlayer(args)) {
			player->setMap(warpTo->getMapId());
		}
		else {
			PlayerPacket::showMessage(player, "Player not found: " + args, PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}