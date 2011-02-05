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
#pragma once

#include "ChatHandlerConstants.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

class Player;
class PacketReader;

namespace ChatHandler {
	struct ChatCommand {
		ChatCommand addToMap() {
			// Duplicates the command and then clears the data to ease addition syntax
			ChatCommand x = *this;
			notes.clear();
			syntax = "";
			return x;
		}
		Commands command;
		int32_t level;
		string syntax;
		vector<string> notes;
	};
	extern unordered_map<string, ChatCommand> commandlist;

	void initializeCommands();
	void showSyntax(Player *player, const string &command, bool fromHelp = false);
	void handleChat(Player *player, PacketReader &packet);
	bool handleCommand(Player *player, const string &message);
	int8_t getMessageType(const string &query);
	int32_t getMap(const string &query, Player *player);
	int16_t getJob(const string &query);
	string getBanString(int8_t reason);
	void handleGroupChat(Player *player, PacketReader &packet);
};
