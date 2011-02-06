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
#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include "ChatHandlerConstants.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::tr1::unordered_map;
using std::vector;

class Player;
class PacketReader;

namespace ChatHandler {
	extern unordered_map<string, pair<Commands, int32_t> > commandlist;
	extern unordered_map<string, string> commandsyntax;
	extern unordered_map<string, vector<string> > commandnotes;

	void initializeCommands();
	void showSyntax(Player *player, const string &command, bool fromHelp = false);
	void handleChat(Player *player, PacketReader &packet);
	bool handleCommand(Player *player, const string &message);
	int32_t getMap(const string &query, Player *player);
	string getBanString(int8_t reason);
	void handleGroupChat(Player *player, PacketReader &packet);
};

#endif