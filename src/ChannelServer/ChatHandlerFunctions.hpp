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
#pragma once

#include "Types.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

class Player;

using ChatHandlerFunction = bool(*)(Player *, const string_t &args);
struct ChatCommand {
	auto addToMap() -> ChatCommand {
		// Duplicates the command and then clears the data to ease addition syntax, intentionally leaving out level
		ChatCommand x = *this;
		notes.clear();
		syntax = "";
		return x;
	}

	ChatHandlerFunction command;
	int32_t level = 0;
	string_t syntax;
	vector_t<string_t> notes;
};

struct MapPair {
	MapPair(int32_t mapId, string_t category) :
		mapId(mapId),
		category(category)
	{
	}

	int32_t mapId;
	string_t category;
};

namespace ChatHandlerFunctions {
	extern case_insensitive_hash_map_t<ChatCommand> sCommandList;
	extern const case_insensitive_hash_map_t<MapPair> sMapAssociations;

	auto initialize() -> void;
	auto getMessageType(const string_t &query) -> int8_t;
	auto getMap(const string_t &query, Player *player) -> int32_t;
	auto getJob(const string_t &query) -> int16_t;
	auto getBanString(int8_t reason) -> string_t;
	auto runRegexPattern(const string_t &args, const string_t &pattern, match_t &matches) -> bool;
	auto showSyntax(Player *player, const string_t &command, bool fromHelp = false) -> void;
	auto showError(Player *player, const string_t &message) -> void;
	auto showInfo(Player *player, const string_t &message) -> void;
	auto showError(Player *player, function_t<void(out_stream_t &)> produceMessage) -> void;
	auto showInfo(Player *player, function_t<void(out_stream_t &)> produceMessage) -> void;
	auto showError(Player *player, const char *message) -> void;
	auto showInfo(Player *player, const char *message) -> void;
}