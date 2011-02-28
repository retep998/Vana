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
#include "CommandHandler.h"
#include "ChatHandler.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "GmPacket.h"
#include "Inventory.h"
#include "Player.h"
#include "PlayerInventory.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include "PlayersPacket.h"
#include "PacketReader.h"
#include "Map.h"
#include "Maps.h"
#include "WorldServerConnectPacket.h"
#include <string>

using std::string;

void CommandHandler::handleCommand(Player *player, PacketReader &packet) {
	uint8_t type = packet.get<int8_t>();
	string name = packet.getString();

	string chat;
	if (type == 0x06) {
		chat = packet.getString();
	}

	Player *receiver = PlayerDataProvider::Instance()->getPlayer(name);
	if (receiver) {
		if (type == 0x05) {
			PlayersPacket::findPlayer(player, receiver->getName(), receiver->getMap());
		}
		else if (type == 0x06) {
			PlayersPacket::whisperPlayer(receiver, player->getName(), ChannelServer::Instance()->getChannel(), chat);
			PlayersPacket::findPlayer(player, receiver->getName(), -1, 1);
		}
	}
	else { // Let's connect to the world server to see if the player is on any other channel
		if (type == 0x05) {
			WorldServerConnectPacket::findPlayer(ChannelServer::Instance()->getWorldConnection(), player->getId(), name);
		}
		else if (type == 0x06) {
			WorldServerConnectPacket::whisperPlayer(ChannelServer::Instance()->getWorldConnection(), player->getId(), name, chat);
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
		case 0x01: { // /d (inv)
			// Destroys the FIRST item found in the inventory
			int8_t inv = packet.get<int8_t>();
			if (!GameLogicUtilities::isValidInventory(inv))
				return;
			uint8_t slots = player->getInventory()->getMaxSlots(inv);
			for (int8_t i = 0; i < slots; i++) {
				if (Item *item = player->getInventory()->getItem(inv, i)) {
					Inventory::takeItemSlot(player, inv, i, player->getInventory()->getItemAmountBySlot(inv, i));
					break;
				}
			}
		}
		break;
		case 0x02: { // /exp (amount)
			int32_t amount = packet.get<int32_t>();
			player->getStats()->giveExp(amount);
		}
		break;
		case 0x03: { // /ban (character name)
			string victim = packet.getString();
			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(victim)) {
				receiver->getSession()->disconnect();
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
		}
		break;
		case 0x04: { // /block (character name) (duration) (sort)
			// Pops up the GM Tool too when used!
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
				PlayersPacket::showMessage(banmsg, 0);
			}
			else {
				GmPacket::invalidCharacterName(player); // Invalid character name
			}
		}
		break;
		case 0x11: PlayerPacket::showMessage(player, Maps::getMap(player->getMap())->getPlayerNames(), 0); break;
		case 0x1D: { // /w (character name) (message)
			string victim = packet.getString();
			string message = packet.getString();
			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(victim)) {
				PlayerPacket::showMessage(receiver, message, 1);
				GmPacket::warning(player, true);
			}
			else {
				GmPacket::warning(player, false);
			}
		}
		break;
		default: {
			std::cout << "Unknown type of Admin command: 0x" << std::hex << static_cast<uint16_t>(type) << ", sent by " << player->getName() << std::endl;
			packet.reset(2);
			size_t length = packet.getBufferLength();
			std::cout << "Packet data: " << std::endl;
			for (size_t i = 0; i < length; i++) {
				std::cout << std::setw(2)
					<< std::setfill('0') << std::internal
					<< std::hex << static_cast<uint16_t>(packet.get<uint8_t>())
					<< " ";
			}
			std::cout << std::endl;
		}
		break;
	}

}
