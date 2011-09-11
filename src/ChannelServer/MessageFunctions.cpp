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
#include "MessageFunctions.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

namespace Functors {
	struct MeFunctor {
		void operator() (Player *gmplayer) {
			if (gmplayer->isGm()) {
				PlayerPacket::showMessage(gmplayer, msg, PlayerPacket::NoticeTypes::Blue);
			}
		}
		string msg;
	};
}

bool MessageFunctions::worldMessage(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (.+)", matches)) {
		int8_t type = ChatHandlerFunctions::getMessageType((string) matches[1]);
		if (type != -1) {
			PlayerPacket::showMessageWorld((string) matches[2], type);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, box, red, blue}", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool MessageFunctions::globalMessage(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\w+) (.+)", matches)) {
		int8_t type = ChatHandlerFunctions::getMessageType((string) matches[1]);
		if (type != -1) {
			PlayerPacket::showMessageGlobal((string) matches[2], type);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, box, red, blue}", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}

bool MessageFunctions::channelMessage(Player *player, const string &args) {
	if (args.length() != 0) {
		PlayerPacket::showMessageChannel(args, PlayerPacket::NoticeTypes::Notice);
		return true;
	}
	return false;
}

bool MessageFunctions::gmMessage(Player *player, const string &args) {
	if (args.length() != 0) {
		string &msg = player->getName() + " : " + args;
		Functors::MeFunctor func = {msg};
		PlayerDataProvider::Instance()->run(func);
		return true;
	}
	return false;
}