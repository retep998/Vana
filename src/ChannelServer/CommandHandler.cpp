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
#include "CommandHandler.h"
#include "Buffs.h"
#include "ChatHandlerFunctions.h"
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
#include "PlayerInventory.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include "PlayersPacket.h"
#include "PacketReader.h"
#include "Skills.h"
#include "WorldServerConnectPacket.h"
#include <string>

namespace CommandOpcodes {
	enum Opcodes : int8_t {
		FindPlayer = 0x05,
		Whisper = 0x06
	};
}

namespace AdminOpcodes {
	enum Opcodes : int8_t {
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
		Snow = 0x1C,
		Warn = 0x1D,
		Log = 0x1E,
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
		Send = /send (character name) (mapId)
		Summon = /summon (mobId) (amount)
		VarSetGet = /varset (charactername) (variable name) (variable value)
					/varget (charactername) (variable name)
		Warn = /w (character name) (message)
		Snow = /snow (time in seconds, minimum: 30, maximum: 600)
	*/
}

auto CommandHandler::handleCommand(Player *player, PacketReader &packet) -> void {
	int8_t type = packet.get<int8_t>();
	const string_t &name = packet.getString();
	Player *receiver = PlayerDataProvider::getInstance().getPlayer(name);
	// If this player doesn't exist, connect to the world server to see if they're on any other channel
	switch (type) {
		case CommandOpcodes::FindPlayer:
			if (receiver != nullptr) {
				if (receiver->isUsingGmHide()) {
					PlayersPacket::findPlayer(player, receiver->getName(), -1, 0);
				}
				else {
					PlayersPacket::findPlayer(player, receiver->getName(), receiver->getMapId());
				}
			}
			else {
				WorldServerConnectPacket::findPlayer(player->getId(), name);
			}
			break;
		case CommandOpcodes::Whisper: {
			const string_t &chat = packet.getString();
			if (receiver) {
				PlayersPacket::whisperPlayer(receiver, player->getName(), ChannelServer::getInstance().getChannelId(), chat);
				PlayersPacket::findPlayer(player, receiver->getName(), -1, 1);
			}
			else {
				WorldServerConnectPacket::whisperPlayer(player->getId(), name, chat);
			}
			break;
		}
	}
}

auto CommandHandler::handleAdminCommand(Player *player, PacketReader &packet) -> void {
	if (!player->isAdmin()) {
		// Hacking
		return;
	}
	int8_t type = packet.get<int8_t>();

	switch (type) {
		case AdminOpcodes::Hide: {
			bool hide = packet.get<bool>();
			if (hide) {
				MapPacket::removePlayer(player);
				GmPacket::beginHide(player);
				Buffs::addBuff(player, Skills::SuperGm::Hide, player->getSkills()->getSkillLevel(Skills::SuperGm::Hide), 0);
			}
			else {
				Skills::stopSkill(player, Skills::SuperGm::Hide);
			}
			break;
		}
		case AdminOpcodes::Send: {
			const string_t &name = packet.getString();
			int32_t mapId = packet.get<int32_t>();

			if (Player *receiver = PlayerDataProvider::getInstance().getPlayer(name)) {
				receiver->setMap(mapId);
			}
			else {
				GmPacket::invalidCharacterName(player);
			}

			break;
		}
		case AdminOpcodes::Summon: {
			int32_t mobId = packet.get<int32_t>();
			int32_t count = packet.get<int32_t>();
			if (MobDataProvider::getInstance().mobExists(mobId)) {
				for (int32_t i = 0; i < count && i < 100; i++) {
					player->getMap()->spawnMob(mobId, player->getPos());
				}
			}
			else {
				ChatHandlerFunctions::showError(player, "Invalid mob: " + std::to_string(mobId));
			}
			break;
		}
		case AdminOpcodes::CreateItem: {
			int32_t itemId = packet.get<int32_t>();
			Inventory::addNewItem(player, itemId, 1);
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
			const string_t &victim = packet.getString();
			if (Player *receiver = PlayerDataProvider::getInstance().getPlayer(victim)) {
				receiver->getSession()->disconnect();
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
		case AdminOpcodes::Block: {
			const string_t &victim = packet.getString();
			int8_t reason = packet.get<int8_t>();
			int32_t length = packet.get<int32_t>();
			const string_t &reasonMessage = packet.getString();
			if (Player *receiver = PlayerDataProvider::getInstance().getPlayer(victim)) {
				Database::getCharDb().once
					<< "UPDATE user_accounts u "
					<< "INNER JOIN characters c ON u.user_id = c.user_id "
					<< "SET "
					<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY),"
					<< "	u.ban_reason = :reason,"
					<< "	u.ban_reasonMessage = :reasonMessage "
					<< "WHERE c.name = :name ",
					soci::use(victim, "name"),
					soci::use(length, "expire"),
					soci::use(reason, "reason"),
					soci::use(reasonMessage, "reasonMessage");

				GmPacket::block(player);
				const string_t &banMessage = victim + " has been banned" + ChatHandlerFunctions::getBanString(reason);
				PlayerPacket::showMessageChannel(banMessage, PlayerPacket::NoticeTypes::Notice);
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
		case AdminOpcodes::ShowMessageMap:
			PlayerPacket::showMessage(player, player->getMap()->getPlayerNames(), PlayerPacket::NoticeTypes::Notice);
			break;
		case AdminOpcodes::Snow:
			player->getMap()->createWeather(player, true, packet.get<int32_t>(), Items::SnowySnow, "");
			break;
		case AdminOpcodes::VarSetGet: {
			int8_t type = packet.get<int8_t>();
			const string_t &playerName = packet.getString();
			if (Player *victim = PlayerDataProvider::getInstance().getPlayer(playerName)) {
				const string_t &variableName = packet.getString();
				if (type == 0x0a) {
					const string_t &variableValue = packet.getString();
					victim->getVariables()->setVariable(variableName, variableValue);
				}
				else {
					GmPacket::setGetVarResult(player, playerName, variableName, victim->getVariables()->getVariable(variableName));
				}
			}
			else {
				GmPacket::invalidCharacterName(player);
			}
			break;
		}
		case AdminOpcodes::Warn: {
			const string_t &victim = packet.getString();
			const string_t &message = packet.getString();
			if (Player *receiver = PlayerDataProvider::getInstance().getPlayer(victim)) {
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