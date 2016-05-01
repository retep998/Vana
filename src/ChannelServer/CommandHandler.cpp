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
#include "CommandHandler.hpp"
#include "Common/Algorithm.hpp"
#include "Common/Database.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/InterHeader.hpp"
#include "Common/MobDataProvider.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/PacketReader.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/ChatHandlerFunctions.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/GmPacket.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/Map.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerInventory.hpp"
#include "ChannelServer/PlayerPacket.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayersPacket.hpp"
#include "ChannelServer/Skills.hpp"
#include "ChannelServer/WorldServerPacket.hpp"
#include <string>

namespace Vana {
namespace ChannelServer {

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

auto CommandHandler::handleCommand(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	chat_t name = reader.get<chat_t>();
	auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(name);
	// If this player doesn't exist, connect to the world server to see if they're on any other channel
	switch (type) {
		case CommandOpcodes::FindPlayer: {
			bool found = false;
			if (receiver != nullptr) {
				if (!receiver->isUsingGmHide() || player->isGm() || player->isAdmin()) {
					player->send(Packets::Players::findPlayer(receiver->getName(), receiver->getMapId()));
					found = true;
				}
			}
			else {
				auto targetData = ChannelServer::getInstance().getPlayerDataProvider().getPlayerDataByName(name);
				if (targetData != nullptr && targetData->channel.is_initialized()) {
					player->send(Packets::Players::findPlayer(targetData->name, targetData->channel.get(), 1, true));
					found = true;
				}
			}
			if (!found) {
				player->send(Packets::Players::findPlayer(name, opt_int32_t{}, 0));
			}
			break;
		}
		case CommandOpcodes::Whisper: {
			chat_t chat = reader.get<chat_t>();
			bool found = false;
			if (receiver != nullptr) {
				receiver->send(Packets::Players::whisperPlayer(player->getName(), ChannelServer::getInstance().getChannelId(), chat));
				player->send(Packets::Players::findPlayer(receiver->getName(), opt_int32_t{}, 1));
				found = true;
			}
			else {
				auto targetData = ChannelServer::getInstance().getPlayerDataProvider().getPlayerDataByName(name);
				if (targetData != nullptr && targetData->channel.is_initialized()) {
					player->send(Packets::Players::findPlayer(targetData->name, opt_int32_t{}, 1));
					ChannelServer::getInstance().sendWorld(
						Vana::Packets::prepend(Packets::Players::whisperPlayer(player->getName(), ChannelServer::getInstance().getChannelId(), chat), [targetData](PacketBuilder &builder) {
							builder.add<header_t>(IMSG_TO_CHANNEL);
							builder.add<channel_id_t>(targetData->channel.get());
							builder.add<header_t>(IMSG_TO_PLAYER);
							builder.add<player_id_t>(targetData->id);
						}));
					found = true;
				}
			}
			if (!found) {
				player->send(Packets::Players::findPlayer(name, opt_int32_t{}, 0));
			}
			break;
		}
	}
}

auto CommandHandler::handleAdminCommand(ref_ptr_t<Player> player, PacketReader &reader) -> void {
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
					Vana::Skills::SuperGm::Hide,
					1,
					0);

				if (result == Result::Successful) {
					player->send(Packets::Gm::beginHide());
					player->getMap()->gmHideChange(player);
				}
			}
			else {
				Skills::stopSkill(
					player,
					BuffSource::fromSkill(
						Vana::Skills::SuperGm::Hide,
						1));
			}
			break;
		}
		case AdminOpcodes::Send: {
			chat_t name = reader.get<chat_t>();
			map_id_t mapId = reader.get<map_id_t>();

			if (auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(name)) {
				receiver->setMap(mapId);
			}
			else {
				player->send(Packets::Gm::invalidCharacterName());
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
			if (auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				receiver->disconnect();
			}
			else {
				player->send(Packets::Gm::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::Block: {
			chat_t victim = reader.get<chat_t>();
			int8_t reason = reader.get<int8_t>();
			int32_t length = reader.get<int32_t>();
			chat_t reasonMessage = reader.get<chat_t>();
			if (auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				auto &db = Database::getCharDb();
				auto &sql = db.getSession();
				sql.once
					<< "UPDATE " << db.makeTable("accounts") << " u "
					<< "INNER JOIN " << db.makeTable("characters") << " c ON u.account_id = c.account_id "
					<< "SET "
					<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY),"
					<< "	u.ban_reason = :reason,"
					<< "	u.ban_reasonMessage = :reasonMessage "
					<< "WHERE c.name = :name ",
					soci::use(victim, "name"),
					soci::use(length, "expire"),
					soci::use(reason, "reason"),
					soci::use(reasonMessage, "reasonMessage");

				player->send(Packets::Gm::block());
				chat_t banMessage = victim + " has been banned" + ChatHandlerFunctions::getBanString(reason);
				ChannelServer::getInstance().getPlayerDataProvider().send(Packets::Player::showMessage(banMessage, Packets::Player::NoticeTypes::Notice));
			}
			else {
				player->send(Packets::Gm::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::ShowMessageMap:
			player->send(Packets::Player::showMessage(player->getMap()->getPlayerNames(), Packets::Player::NoticeTypes::Notice));
			break;
		case AdminOpcodes::Snow:
			player->getMap()->createWeather(player, true, reader.get<int32_t>(), Items::SnowySnow, "");
			break;
		case AdminOpcodes::VarSetGet: {
			int8_t type = reader.get<int8_t>();
			chat_t playerName = reader.get<chat_t>();
			if (auto victim = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerName)) {
				chat_t variableName = reader.get<chat_t>();
				if (type == 0x0a) {
					chat_t variableValue = reader.get<chat_t>();
					victim->getVariables()->setVariable(variableName, variableValue);
				}
				else {
					player->send(Packets::Gm::setGetVarResult(playerName, variableName, victim->getVariables()->getVariable(variableName)));
				}
			}
			else {
				player->send(Packets::Gm::invalidCharacterName());
			}
			break;
		}
		case AdminOpcodes::Warn: {
			chat_t victim = reader.get<chat_t>();
			chat_t message = reader.get<chat_t>();
			if (auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(victim)) {
				receiver->send(Packets::Player::showMessage(message, Packets::Player::NoticeTypes::Box));
				player->send(Packets::Gm::warning(true));
			}
			else {
				player->send(Packets::Gm::warning(false));
			}
			break;
		}
	}
}

}
}