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
#include "ChannelServer.hpp"
#include "ChatHandlerFunctions.hpp"
#include "Map.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "WorldServerConnectPacket.hpp"
#include <algorithm>

auto ChatHandler::initializeCommands() -> void {
	ChatHandlerFunctions::initialize();
}

auto ChatHandler::handleChat(Player *player, PacketReader &reader) -> void {
	chat_t message = reader.get<chat_t>();
	bool bubbleOnly = reader.get<bool>(); // Skill macros only display chat bubbles

	if (ChatHandler::handleCommand(player, message) == HandleResult::Unhandled) {
		if (player->isGmChat()) {
			ChannelServer::getInstance().getPlayerDataProvider().handleGmChat(player, message);
			return;
		}

		player->sendMap(PlayersPacket::showChat(player->getId(), player->isGm(), message, bubbleOnly));
	}
}

auto ChatHandler::handleCommand(Player *player, const chat_t &message) -> HandleResult {
	using ChatHandlerFunctions::sCommandList;

	if (player->isAdmin() && message[0] == '/') {
		// Prevent command printing for Admins
		return HandleResult::Handled;
	}

	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		chat_t command = strtok(chat + 1, " ");
		chat_t args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		auto kvp = sCommandList.find(command);
		if (kvp == std::end(sCommandList)) {
			ChatHandlerFunctions::showError(player, "Invalid command: " + command);
		}
		else {
			auto &cmd = kvp->second;
			if (player->getGmLevel() < cmd.level) {
				ChatHandlerFunctions::showError(player, "You are not at a high enough GM level to use the command");
			}
			else if (cmd.command(player, args) == ChatResult::ShowSyntax) {
				ChatHandlerFunctions::showSyntax(player, command);
			}
		}
		return HandleResult::Handled;
	}

	return HandleResult::Unhandled;
}

auto ChatHandler::handleGroupChat(Player *player, PacketReader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	uint8_t amount = reader.get<uint8_t>();
	vector_t<player_id_t> receivers = reader.get<vector_t<player_id_t>>(amount);
	chat_t chat = reader.get<chat_t>();

	if (ChatHandler::handleCommand(player, chat) == HandleResult::Unhandled) {
		ChannelServer::getInstance().getPlayerDataProvider().handleGroupChat(type, player->getId(), receivers, chat);
	}
}