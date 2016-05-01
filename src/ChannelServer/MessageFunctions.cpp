/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "MessageFunctions.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketWrapper.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayerPacket.hpp"

namespace Vana {
namespace ChannelServer {

auto MessageFunctions::worldMessage(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches) == MatchResult::AnyMatches) {
		string_t rawType = matches[1];
		int8_t type = ChatHandlerFunctions::getMessageType(rawType);
		if (type != -1) {
			string_t message = matches[2];
			ChannelServer::getInstance().sendWorld(
				Vana::Packets::prepend(Packets::Player::showMessage(message, type), [](PacketBuilder &builder) {
					builder.add<header_t>(IMSG_TO_ALL_CHANNELS);
					builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
				}));
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid message type: " + rawType);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MessageFunctions::globalMessage(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches) == MatchResult::AnyMatches) {
		string_t rawType = matches[1];
		int8_t type = ChatHandlerFunctions::getMessageType(rawType);
		if (type != -1) {
			string_t message = matches[2];
			ChannelServer::getInstance().sendWorld(
				Vana::Packets::prepend(Packets::Player::showMessage(message, type), [](PacketBuilder &builder) {
					builder.add<header_t>(IMSG_TO_LOGIN);
					builder.add<header_t>(IMSG_TO_ALL_WORLDS);
					builder.add<header_t>(IMSG_TO_ALL_CHANNELS);
					builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
				}));
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid message type: " + rawType);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto MessageFunctions::channelMessage(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\w+) (.+))", matches) == MatchResult::AnyMatches) {
		string_t rawType = matches[1];
		int8_t type = ChatHandlerFunctions::getMessageType(rawType);
		if (type != -1) {
			string_t message = matches[2];
			ChannelServer::getInstance().getPlayerDataProvider().send(Packets::Player::showMessage(message, type));
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid message type: " + rawType);
		}
		return ChatResult::HandledDisplay;
	}

	return ChatResult::ShowSyntax;
}

auto MessageFunctions::gmChatMode(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult {
	player->setGmChat(!player->isGmChat());
	ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) { message << "GM chat mode " << (player->isGmChat() ? "enabled" : "disabled"); });
	return ChatResult::HandledDisplay;
}

}
}