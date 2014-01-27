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
#include "MessageFunctions.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"

auto MessageFunctions::worldMessage(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches)) {
		string_t rawType = matches[1];
		int8_t type = ChatHandlerFunctions::getMessageType(rawType);
		if (type != -1) {
			string_t message = matches[2];
			PlayerPacket::showMessageWorld(message, type);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid message type: " + rawType);
		}
		return true;
	}
	return false;
}

auto MessageFunctions::globalMessage(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches)) {
		string_t rawType = matches[1];
		int8_t type = ChatHandlerFunctions::getMessageType(rawType);
		if (type != -1) {
			string_t message = matches[2];
			PlayerPacket::showMessageGlobal(message, type);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid message type: " + rawType);
		}
		return true;
	}
	return false;
}

auto MessageFunctions::channelMessage(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		PlayerPacket::showMessageChannel(args, PlayerPacket::NoticeTypes::Notice);
		return true;
	}
	return false;
}

auto MessageFunctions::gmMessage(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		const string_t &msg = player->getName() + " : " + args;
		PlayerDataProvider::getInstance().run([&msg](Player *gmPlayer) {
			if (gmPlayer->isGm()) {
				ChatHandlerFunctions::showInfo(gmPlayer, msg);
			}
		});
		return true;
	}
	return false;
}