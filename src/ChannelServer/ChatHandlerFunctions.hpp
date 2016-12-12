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

enum class ChatResult {
	HandledDisplay,
	ShowSyntax,
};

using ChatHandlerFunction = ChatResult(*)(Player *, const chat_t &args);
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
	chat_t syntax;
	vector_t<chat_t> notes;
};

struct MapPair {
	MapPair(map_id_t mapId, chat_t category) :
		mapId(mapId),
		category(category)
	{
	}

	map_id_t mapId;
	chat_t category;
};

namespace ChatHandlerFunctions {
	extern case_insensitive_hash_map_t<ChatCommand, chat_t> sCommandList;
	extern const case_insensitive_hash_map_t<MapPair, chat_t> sMapAssociations;

	auto initialize() -> void;
	auto getMessageType(const chat_t &query) -> int8_t;
	auto getMap(const chat_t &query, Player *player) -> map_id_t;
	auto getJob(const chat_t &query) -> job_id_t;
	auto getBanString(int8_t reason) -> chat_t;
	auto runRegexPattern(const chat_t &args, const chat_t &pattern, match_t &matches) -> MatchResult;
	auto showSyntax(Player *player, const chat_t &command, bool fromHelp = false) -> void;
	auto showError(Player *player, const chat_t &message) -> void;
	auto showInfo(Player *player, const chat_t &message) -> void;
	auto showError(Player *player, function_t<void(chat_stream_t &)> produceMessage) -> void;
	auto showInfo(Player *player, function_t<void(chat_stream_t &)> produceMessage) -> void;
	auto showError(Player *player, const char *message) -> void;
	auto showInfo(Player *player, const char *message) -> void;
}