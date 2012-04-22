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
#include "ChatHandler.h"
#include "ChannelServer.h"
#include "ChatHandlerFunctions.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Session.h"
#include "StringUtilities.h"
#include "WorldServerConnectPacket.h"
#include <algorithm>

using std::vector;

void ChatHandler::initializeCommands() {
	ChatHandlerFunctions::initialize();
}

void ChatHandler::handleChat(Player *player, PacketReader &packet) {
	const string &message = packet.getString();
	bool bubbleOnly = packet.getBool(); // Skill macros only display chat bubbles

	if (!ChatHandler::handleCommand(player, message)) {
		PlayersPacket::showChat(player, message, bubbleOnly);
	}
}

bool ChatHandler::handleCommand(Player *player, const string &message) {
	using ChatHandlerFunctions::CommandList;

	if (player->isAdmin() && message[0] == '/') {
		// Prevent command printing for Admins
		return true;
	}
	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string command = strtok(chat + 1, " ");
		const string &args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		command = StringUtilities::toLower(command);

		if (CommandList.find(command) == CommandList.end()) {
			PlayerPacket::showMessage(player, "Command \"" + command + "\" does not exist.", PlayerPacket::NoticeTypes::Red);
		}
		else {
			ChatCommand &cmd = CommandList[command];
			if (player->getGmLevel() < cmd.level) {
				PlayerPacket::showMessage(player, "You are not at a high enough GM level to use the command.", PlayerPacket::NoticeTypes::Red);
			}
			else if (!cmd.command(player, args)) {
				ChatHandlerFunctions::showSyntax(player, command);
			}
		}
		return true;
	}
	return false;
}

void ChatHandler::handleGroupChat(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	uint8_t amount = packet.get<uint8_t>();
	const vector<int32_t> &receivers = packet.getVector<int32_t>(amount);
	const string &chat = packet.getString();

	if (!ChatHandler::handleCommand(player, chat)) {
		WorldServerConnectPacket::groupChat(ChannelServer::Instance()->getWorldConnection(), type, player->getId(), receivers, chat);
	}
}