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
#pragma once

#include "Types.h"
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::cmatch;
using std::regex;
using std::vector;

class Player;

typedef bool (*ChatHandlerFunction)(Player *, const string &args);
struct ChatCommand {
	ChatCommand addToMap() {
		// Duplicates the command and then clears the data to ease addition syntax
		ChatCommand x = *this;
		notes.clear();
		syntax = "";
		return x;
	}
	ChatHandlerFunction command;
	int32_t level;
	string syntax;
	vector<string> notes;
};
typedef std::unordered_map<string, ChatCommand> CommandListType;

namespace ChatHandlerFunctions {
	extern CommandListType CommandList;
	void initialize();
	int8_t getMessageType(const string &query);
	int32_t getMap(const string &query, Player *player);
	int16_t getJob(const string &query);
	string getBanString(int8_t reason);
	bool runRegexPattern(const string &args, const string &pattern, cmatch &matches);
	void showSyntax(Player *player, const string &command, bool fromHelp = false);
}