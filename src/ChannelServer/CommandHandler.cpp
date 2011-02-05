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
#include "Buffs.h"
#include "ChatHandler.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "GmPacket.h"
#include "Inventory.h"
#include "Map.h"
#include "MapPacket.h"
#include "Maps.h"
#include "MobDataProvider.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerInventory.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "PacketReader.h"
#include "Skills.h"
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
		CreateItem = 0x00,
		DestroyFirstItem = 0x01,
		GiveExp = 0x02,
		Ban = 0x03,
		Block = 0x04,
		VarSetGet = 0x09,
		Hide = 0x10,
		ShowMessageMap = 0x11,
		Send = 0x12,
		Summon = 0x17,
		Snow = 0x1c,
		Warn = 0x1d,
		Log = 0x1e,
		SetObjState = 0x22
	};
	/*
		Opcode syntax:
		DestroyFirstItem = /d (inv)
		GiveExp = /exp (amount)
		Ban = /ban (character name)
		Block = /block (character name) (duration) (sort)
		Hide = /h (0 = off, 1 = on)
		Log = /log (character name) (0 = off, 1 = on)
		Send = /send (character name) (mapid)
		Summon = /summon (mobid) (amount)
		VarSetGet = /varset (charactername) (variable name) (variable value)
					/varget (charactername) (variable name)
		Warn = /w (character name) (message)
		Snow = /snow (time in seconds, minimum: 30, maximum: 600)
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
				PlayersPacket::findPlayer(player, receiver->getName(), receiver->getMap(), 0, true);
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
		case AdminOpcodes::Hide: {
			bool hide = packet.getBool();
			if (hide) {
				MapPacket::removePlayer(player);
				GmPacket::beginHide(player);
				Buffs::addBuff(player, Jobs::SuperGm::Hide, player->getSkills()->getSkillLevel(Jobs::SuperGm::Hide), 0);
			}
			else {
				Skills::stopSkill(player, Jobs::SuperGm::Hide);
			}
			break;
		}
		case AdminOpcodes::Send: {
			string name = packet.getString();
			int32_t mapid = packet.get<int32_t>();

			if (Player *receiver = PlayerDataProvider::Instance()->getPlayer(name)) {
				receiver->setMap(mapid);
			}
			else {
				GmPacket::invalidCharacterName(player);
			}

			break;
		}
		case AdminOpcodes::Summon: {
			int32_t mobid = packet.get<int32_t>();
			int32_t count = packet.get<int32_t>();
			if (MobDataProvider::Instance()->mobExists(mobid)) {
				for (int32_t i = 0; i < count && i < 100; i++) {
					Maps::getMap(player->getMap())->spawnMob(mobid, player->getPos());
				}
			}
			else {
				PlayerPacket::showMessage(player, "Invalid Mob ID", PlayerPacket::NoticeTypes::Blue);
			}
			break;
		}
		case AdminOpcodes::CreateItem: {
			int32_t itemid = packet.get<int32_t>();
			Inventory::addNewItem(player, itemid, 1);
			break;
		}
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
		case AdminOpcodes::Snow:
			Maps::getMap(player->getMap())->createWeather(player, true, packet.get<int32_t>(), Items::SnowySnow, "");
			break;
		case AdminOpcodes::VarSetGet: {
			int8_t type = packet.get<int8_t>();
			string PlayerName = packet.getString();
			if (Player *victim = PlayerDataProvider::Instance()->getPlayer(PlayerName)) {
				string VariableName = packet.getString();
				if (type == 0x0a) {
					string VariableValue = packet.getString();
					victim->getVariables()->setVariable(VariableName, VariableValue);
				}
				GmPacket::setGetVarResult(player, PlayerName, VariableName, victim->getVariables()->getVariable(VariableName));
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
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
