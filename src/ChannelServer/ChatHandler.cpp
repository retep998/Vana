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
#include "ChatHandler.hpp"
#include "ChatHandlerFunctions.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "WorldServerConnectPacket.hpp"
#include <algorithm>

auto ChatHandler::initializeCommands() -> void {
	ChatHandlerFunctions::initialize();
}

auto ChatHandler::handleChat(Player *player, PacketReader &packet) -> void {
	string_t message = packet.getString();
	bool bubbleOnly = packet.get<bool>(); // Skill macros only display chat bubbles

	if (!ChatHandler::handleCommand(player, message)) {
		PlayersPacket::showChat(player, message, bubbleOnly);
	}
}

auto ChatHandler::handleCommand(Player *player, const string_t &message) -> bool {
	using ChatHandlerFunctions::sCommandList;

	if (player->isAdmin() && message[0] == '/') {
		// Prevent command printing for Admins
		return true;
	}
	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string_t command = strtok(chat + 1, " ");
		string_t args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		auto kvp = sCommandList.find(command);
		if (kvp == std::end(sCommandList)) {
			ChatHandlerFunctions::showError(player, "Invalid command: " + command);
		}
		else {
			auto &cmd = kvp->second;
			if (player->getGmLevel() < cmd.level) {
				ChatHandlerFunctions::showError(player, "You are not at a high enough GM level to use the command");
			}
			else if (!cmd.command(player, args)) {
				ChatHandlerFunctions::showSyntax(player, command);
			}
		}
		return true;
	}
	return false;
}

auto ChatHandler::handleGroupChat(Player *player, PacketReader &packet) -> void {
	int8_t type = packet.get<int8_t>();
	uint8_t amount = packet.get<uint8_t>();
	vector_t<int32_t> receivers = packet.getVector<int32_t>(amount);
	string_t chat = packet.getString();

	if (!ChatHandler::handleCommand(player, chat)) {
		WorldServerConnectPacket::groupChat(type, player->getId(), receivers, chat);
	}
}