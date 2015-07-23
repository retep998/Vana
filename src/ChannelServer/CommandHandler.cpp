/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "CommandHandler.hpp"
#include "Algorithm.hpp"
#include "Buffs.hpp"
#include "ChatHandlerFunctions.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "GmPacket.hpp"
#include "InterHeader.hpp"
#include "Inventory.hpp"
#include "Map.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "MobDataProvider.hpp"
#include "PacketWrapper.hpp"
#include "Player.hpp"
#include "PlayerInventory.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayersPacket.hpp"
#include "PacketReader.hpp"
#include "Skills.hpp"
#include "WorldServerConnectPacket.hpp"
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

auto CommandHandler::handleCommand(Player *player, PacketReader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	chat_t name = reader.get<chat_t>();
	Player *receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(name);
	// If this player doesn't exist, connect to the world server to see if they're on any other channel
	switch (type) {
		case CommandOpcodes::FindPlayer: {
			bool found = false;
			if (receiver != nullptr) {
				if (!receiver->isUsingGmHide() || player->isGm() || player->isAdmin()) {
					player->send(PlayersPacket::findPlayer(receiver->getName(), receiver->getMapId()));
					found = true;
				}
			}
			else {
				auto targetData = ChannelServer::getInstance().getPlayerDataProvider().getPlayerDataByName(name);
				if (targetData != nullptr && targetData->channel.is_initialized()) {
					player->send(PlayersPacket::findPlayer(targetData->name, targetData->channel.get(), 1, true));
					found = true;
				}
			}
			if (!found) {
				player->send(PlayersPacket::findPlayer(name, opt_int32_t{}, 0));
			}
			break;
		}
		case CommandOpcodes::Whisper: {
			chat_t chat = reader.get<chat_t>();
			bool found = false;
			if (receiver != nullptr) {
				receiver->send(PlayersPacket::whisperPlayer(player->getName(), ChannelServer::getInstance().getChannelId(), chat));
				player->send(PlayersPacket::findPlayer(receiver->getName(), opt_int32_t{}, 1));
				found = true;
			}
			else {
				auto targetData = ChannelServer::getInstance().getPlayerDataProvider().getPlayerDataByName(name);
				if (targetData != nullptr && targetData->channel.is_initialized()) {
					player->send(PlayersPacket::findPlayer(targetData->name, opt_int32_t{}, 1));
					ChannelServer::getInstance().sendWorld(
						Packets::prepend(PlayersPacket::whisperPlayer(player->getName(), ChannelServer::getInstance().getChannelId(), chat), [targetData](PacketBuilder &builder) {
							builder.add<header_t>(IMSG_TO_CHANNEL);
							builder.add<channel_id_t>(targetData->channel.get());
							builder.add<header_t>(IMSG_TO_PLAYER);
							builder.add<player_id_t>(targetData->id);
						}));
					found = true;
				}
			}
			if (!found) {
				player->send(PlayersPacket::findPlayer(name, opt_int32_t{}, 0));
			}
			break;
		}
	}
}

auto CommandHandler::handleAdminCommand(Player *player, PacketReader &reader) -> void {
	if (!player->isAdmin()) {
		// Hacking
		return;
	}

	int8_t type = reader.get<int8_t>();
	switch (type) {
		case AdminOpcodes::Hide: {
			bool hide = reader.get<bool>();
			if (hide) {
				auto result = Buffs::addBuff(
					player,
					Skills::SuperGm::Hide,
					1,
					0);

				if (result == Result::Successful) {
					player->send(GmPacket::beginHide());
					player->getMap()->gmHideChange(player);
				}
			}
			else {
				Skills::stopSkill(
					player,
					BuffSource::fromSkill(
						Skills::SuperGm::Hide,
						1));
			}
			break;
		}
		case AdminOpcodes::Send: {
			chat_t name = reader.get<chat_t>();
			map_id_t mapId = reader.get<map_id_t>();

			if (Player *receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(name)) {
				receiver->setMap(mapId);
			}
			else {
				player->send(GmPacket::invalidCharacterName());
			}

			break;
		}
		case AdminOpcodes::Summon: {
			mob_id_t mobId = reader.get<mob_id_t>();
			int32_t count = reader.get<int32_t>();
			if (ChannelServer::getInstance().getMobDataProvider().mobExists(mobId)) {
				count = ext::constrain_range(count, 1, 1000);
				for (int32_t i = 0; i < count; ++i) {
					player->getMap()->spawnMob(mobId, player->getPos());
				}
			}
			else {
				ChatHandlerFunctions::showError(player, "Invalid mob: " + std::to_string(mobId));
			}
			break;
		}
		case AdminOpcodes::CreateItem: {
			item_id_t itemId = reader.get<item_id_t>();
			Inventory::addNewItem(player, itemId, 1);
			break;
		}
		case AdminOpcodes::DestroyFirstItem: {
			inventory_t inv = reader.get<inventory_t>();
			if (!GameLogicUtilities::isValidInventory(inv)) {
				return;
			}
			inventory_slot_count_t slots = player->getInventory()->getMaxSlots(inv);
			for (inventory_slot_count_t i = 0; i < slots; ++i) {
				if (Item *item = player->getInventory()->getItem(inv, i)) {
					Inventory::takeItemSlot(player, inv, i, player->getInventory()->getItemAmountBySlot(inv, i));
					break;
				}
			}
			break;
		}
		case AdminOpcodes::GiveExp: {
			experience_t amount = reader.get<experience_t>();
			player->getStats()->giveExp(amount);
			break;
		}
		case AdminOpcodes::Ban: {
			chat_t victim = reader.get<chat_t>();
			if (Player *receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				receiver->disconnect();
			}
			else {
				player->send(GmPacket::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::Block: {
			chat_t victim = reader.get<chat_t>();
			int8_t reason = reader.get<int8_t>();
			int32_t length = reader.get<int32_t>();
			chat_t reasonMessage = reader.get<chat_t>();
			if (Player *receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				Database::getCharDb().once
					<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
					<< "INNER JOIN " << Database::makeCharTable("characters") << " c ON u.user_id = c.user_id "
					<< "SET "
					<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY),"
					<< "	u.ban_reason = :reason,"
					<< "	u.ban_reasonMessage = :reasonMessage "
					<< "WHERE c.name = :name ",
					soci::use(victim, "name"),
					soci::use(length, "expire"),
					soci::use(reason, "reason"),
					soci::use(reasonMessage, "reasonMessage");

				player->send(GmPacket::block());
				chat_t banMessage = victim + " has been banned" + ChatHandlerFunctions::getBanString(reason);
				ChannelServer::getInstance().getPlayerDataProvider().send(PlayerPacket::showMessage(banMessage, PlayerPacket::NoticeTypes::Notice));
			}
			else {
				player->send(GmPacket::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::ShowMessageMap:
			player->send(PlayerPacket::showMessage(player->getMap()->getPlayerNames(), PlayerPacket::NoticeTypes::Notice));
			break;
		case AdminOpcodes::Snow:
			player->getMap()->createWeather(player, true, reader.get<int32_t>(), Items::SnowySnow, "");
			break;
		case AdminOpcodes::VarSetGet: {
			int8_t type = reader.get<int8_t>();
			chat_t playerName = reader.get<chat_t>();
			if (Player *victim = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerName)) {
				chat_t variableName = reader.get<chat_t>();
				if (type == 0x0a) {
					chat_t variableValue = reader.get<chat_t>();
					victim->getVariables()->setVariable(variableName, variableValue);
				}
				else {
					player->send(GmPacket::setGetVarResult(playerName, variableName, victim->getVariables()->getVariable(variableName)));
				}
			}
			else {
				player->send(GmPacket::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::Warn: {
			chat_t victim = reader.get<chat_t>();
			chat_t message = reader.get<chat_t>();
			if (Player *receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				receiver->send(PlayerPacket::showMessage(message, PlayerPacket::NoticeTypes::Box));
				player->send(GmPacket::warning(true));
			}
			else {
				player->send(GmPacket::warning(false));
			}
			break;
		}
	}
}