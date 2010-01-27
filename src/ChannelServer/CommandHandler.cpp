/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "CommandHandler.h"
#include "ChatHandler.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "GmPacket.h"
#include "Inventory.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerInventory.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "PacketReader.h"
#include "Map.h"
#include "Maps.h"
#include "WorldServerConnectPacket.h"
#include <string>

using std::string;

namespace CommandOpcodes {
	enum Opcodes {
		FindPlayer = 0x05,
		Whisper = 0x06
	};
}

namespace AdminOpcodes {
	enum Opcodes {
		DestroyFirstItem = 0x01,
		GiveExp = 0x02,
		Ban = 0x03,
		Block = 0x04,
		ShowMessageMap = 0x11,
		Warn = 0x1D
	};
	/*
		Opcode syntax:
		DestroyFirstItem = /d (inv)
		GiveExp = /exp (amount)
		Ban = /ban (character name)
		Block = /block (character name) (duration) (sort)
		Warn = /w (character name) (message)
	*/
}

void CommandHandler::handleCommand(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	string name = packet.getString();

	Player *receiver = PlayerDataProvider::Instance()->getPlayer(name);
	// If this player doesn't exist, connect to the world server to see if they're on any other channel
	switch (type) {
		case CommandOpcodes::FindPlayer:
			if (receiver) {
				PlayersPacket::findPlayer(player, receiver->getName(), receiver->getMap());
			}
			else {
				WorldServerConnectPacket::findPlayer(ChannelServer::Instance()->getWorldConnection(), player->getId(), name);
			}
			break;
		case CommandOpcodes::Whisper: {
			string chat = packet.getString();
			if (receiver) {
				PlayersPacket::whisperPlayer(receiver, player->getName(), ChannelServer::Instance()->getChannel(), chat);
				PlayersPacket::findPlayer(player, receiver->getName(), -1, 1);
			}
			else {
				WorldServerConnectPacket::whisperPlayer(ChannelServer::Instance()->getWorldConnection(), player->getId(), name, chat);
			}
			break;
		}
	}
}

void CommandHandler::handleAdminCommand(Player *player, PacketReader &packet) {
	if (!player->isAdmin()) {
		// Admin byte hacking!
		player->addWarning();
		return;
	}
	int8_t type = packet.get<int8_t>();

	switch (type) {
		case AdminOpcodes::DestroyFirstItem: {
			int8_t inv = packet.get<int8_t>();
			if (!GameLogicUtilities::isValidInventory(inv)) {
				return;
			}
			uint8_t slots = player->getInventory()->getMaxSlots(inv);
			for (int8_t i = 0; i < slots; i++) {
				if (Item *item = player->getInventory()->getItem(inv, i)) {
					Inventory::takeItemSlot(player, inv, i, player->getInventory()->getItemAmountBySlot(inv, i));
					break;
				}
			}
			break;
		}
		case AdminOpcodes::GiveExp: {
			int32_t amount = packet.get<int32_t>();
			player->getStats()->giveExp(amount);
			break;
		}
		case AdminOpcodes::Ban: {
			string victim = packet.getString();
			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(victim)) {
				receiver->getSession()->disconnect();
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
		case AdminOpcodes::Block: {
			string victim = packet.getString();
			int8_t reason = packet.get<int8_t>();
			int32_t length = packet.get<int32_t>();
			string reason_message = packet.getString();
			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(victim)) {
				mysqlpp::Query accbanquery = Database::getCharDB().query();
				accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET "
					<< "users.ban_reason = " << (int16_t) reason << ", "
					<< "users.ban_expire = DATE_ADD(NOW(), INTERVAL " << length << " DAY), "
					<< "ban_reason_message = " << mysqlpp::quote << reason_message << " WHERE characters.name = '" << victim << "'";
				accbanquery.exec();

				GmPacket::block(player);
				string banmsg = victim + " has been banned" + ChatHandler::getBanString(reason);
				PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
		case AdminOpcodes::ShowMessageMap:
			PlayerPacket::showMessage(player, Maps::getMap(player->getMap())->getPlayerNames(), PlayerPacket::NoticeTypes::Notice);
			break;
		case AdminOpcodes::Warn: {
			string victim = packet.getString();
			string message = packet.getString();
			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(victim)) {
				PlayerPacket::showMessage(receiver, message, PlayerPacket::NoticeTypes::Box);
				GmPacket::warning(player, true);
			}
			else {
				GmPacket::warning(player, false);
			}
			break;
		}
	}
}
