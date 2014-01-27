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
#include "InventoryPacket.h"
#include "ChannelServer.h"
#include "GameLogicUtilities.h"
#include "InterHeader.h"
#include "Inventory.h"
#include "InventoryPacketHelper.h"
#include "ItemConstants.h"
#include "Map.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerInventory.h"
#include "PlayerPacketHelper.h"
#include "Session.h"
#include "SmsgHeader.h"

auto InventoryPacket::updatePlayer(Player *player) -> void {
	if (player->isUsingGmHide()) {
		return;
	}

	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_CHANGE_LOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	PlayerPacketHelper::addPlayerDisplay(packet, player);
	packet.add<int8_t>(0);
	packet.add<int16_t>(0);
	player->getMap()->sendPacket(packet, player);
}

auto InventoryPacket::inventoryOperation(Player *player, bool unk, const vector_t<InventoryPacketOperation> &operations) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_INVENTORY_OPERATION);
	packet.add<bool>(unk);
	packet.add<uint8_t>(operations.size());
	for (const auto &operation : operations) {
		packet.add<int8_t>(operation.operationType);
		packet.add<int8_t>(GameLogicUtilities::getInventory(operation.item->getId()));
		switch (operation.operationType) {
			case InventoryPacket::OperationTypes::AddItem:
				PlayerPacketHelper::addItemInfo(packet, operation.currentSlot, operation.item, true);
				break;
			case InventoryPacket::OperationTypes::ModifyQuantity:
				packet.add<int16_t>(operation.currentSlot);
				packet.add<int16_t>(operation.item->getAmount());
				break;
			case InventoryPacket::OperationTypes::ModifySlot:
				packet.add<int16_t>(operation.oldSlot);
				packet.add<int16_t>(operation.currentSlot);
				if (operation.oldSlot < 0) {
					packet.add<int8_t>(1);
				}
				else if (operation.currentSlot < 0) {
					packet.add<int8_t>(2);
				}
				break;
			case InventoryPacket::OperationTypes::RemoveItem:
				packet.add<int16_t>(operation.currentSlot);
				break;
		}
	}

	player->getSession()->send(packet);
}

auto InventoryPacket::sitChair(Player *player, int32_t chairId) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_CHAIR_SIT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(chairId);
	player->getMap()->sendPacket(packet, player);
}

auto InventoryPacket::sitMapChair(Player *player, int16_t chairId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHAIR);
	packet.add<int8_t>(1);
	packet.add<int16_t>(chairId);
	player->getSession()->send(packet);
}

auto InventoryPacket::stopChair(Player *player, bool showMap) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHAIR);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->isUsingGmHide() || !showMap) {
		return;
	}
	packet = PacketCreator();
	packet.add<header_t>(SMSG_CHAIR_SIT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	player->getMap()->sendPacket(packet, player);
}

auto InventoryPacket::useScroll(Player *player, int8_t succeed, bool destroy, bool legendarySpirit) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_SCROLL_USE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(succeed);
	packet.add<bool>(destroy);
	packet.add<int16_t>(legendarySpirit);
	player->getMap()->sendPacket(packet);
}

auto InventoryPacket::showMegaphone(Player *player, const string_t &msg) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(2);
	packet.addString(msg);
	PlayerDataProvider::getInstance().sendPacket(packet); // In global, this sends to everyone on the current channel, not the map
}

auto InventoryPacket::showSuperMegaphone(Player *player, const string_t &msg, bool whisper) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYERS);
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(3);
	packet.addString(msg);
	packet.add<int8_t>(static_cast<int8_t>(ChannelServer::getInstance().getChannelId()));
	packet.add<bool>(whisper);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto InventoryPacket::showMessenger(Player *player, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, unsigned char *displayInfo, int32_t displayInfoSize, int32_t itemId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYERS);
	packet.add<header_t>(SMSG_AVATAR_MEGAPHONE);
	packet.add<int32_t>(itemId);
	packet.addString(player->getName());
	packet.addString(msg);
	packet.addString(msg2);
	packet.addString(msg3);
	packet.addString(msg4);
	packet.add<int32_t>(ChannelServer::getInstance().getChannelId());
	packet.addBuffer(displayInfo, displayInfoSize);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto InventoryPacket::showItemMegaphone(Player *player, const string_t &msg, bool whisper, Item *item) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYERS);
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(8);
	packet.addString(msg);
	packet.add<int8_t>(static_cast<int8_t>(ChannelServer::getInstance().getChannelId()));
	packet.add<bool>(whisper);
	if (item == nullptr) {
		packet.add<int8_t>(0);
	}
	else {
		PlayerPacketHelper::addItemInfo(packet, 1, item);
	}
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto InventoryPacket::showTripleMegaphone(Player *player, int8_t lines, const string_t &line1, const string_t &line2, const string_t &line3, bool whisper) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYERS);
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(0x0a);
	packet.addString(line1);
	packet.add<int8_t>(lines);
	if (lines > 1) {
		packet.addString(line2);
	}
	if (lines > 2) {
		packet.addString(line3);
	}
	packet.add<int8_t>(static_cast<int8_t>(ChannelServer::getInstance().getChannelId()));
	packet.add<bool>(whisper);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto InventoryPacket::useSkillbook(Player *player, int32_t skillId, int32_t newMaxLevel, bool use, bool succeed) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SKILLBOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1); // Number of skills? Maybe just padding or random boolean
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(newMaxLevel);
	packet.add<bool>(use);
	packet.add<bool>(succeed);
	player->getMap()->sendPacket(packet);
}

auto InventoryPacket::useItemEffect(Player *player, int32_t itemId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_ITEM_EFFECT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(itemId);
	player->getMap()->sendPacket(packet, player);
}

auto InventoryPacket::updateSlots(Player *player, int8_t inventory, int8_t slots) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_INVENTORY_SLOT_UPDATE);
	packet.add<int8_t>(inventory);
	packet.add<int8_t>(slots);
	player->getSession()->send(packet);
}

auto InventoryPacket::blankUpdate(Player *player) -> void {
	vector_t<InventoryPacketOperation> ops;
	inventoryOperation(player, true, ops);
}

auto InventoryPacket::sendRockUpdate(Player *player, int8_t mode, int8_t type, const vector_t<int32_t> &maps) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_TELEPORT_ROCK);
	packet.add<int8_t>(mode);
	packet.add<int8_t>(type);

	InventoryPacketHelper::fillRockPacket(packet, maps, (type == RockTypes::Regular ? Inventories::TeleportRockMax : Inventories::VipRockMax));

	player->getSession()->send(packet);
}

auto InventoryPacket::sendRockError(Player *player, int8_t code, int8_t type) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_TELEPORT_ROCK);
	packet.add<int8_t>(code);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

auto InventoryPacket::sendMesobagSucceed(Player *player, int32_t mesos) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MESOBAG_SUCCESS);
	packet.add<int32_t>(mesos);
	player->getSession()->send(packet);
}

auto InventoryPacket::sendMesobagFailed(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MESOBAG_FAILURE);
	player->getSession()->send(packet);
}

auto InventoryPacket::useCharm(Player *player, uint8_t charmsLeft, uint8_t daysLeft) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(0x01);
	packet.add<uint8_t>(charmsLeft);
	packet.add<uint8_t>(daysLeft);
	player->getSession()->send(packet);
}

auto InventoryPacket::sendHammerSlots(Player *player, int32_t slots) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_HAMMER);
	packet.add<int8_t>(0x34); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	packet.add<int32_t>(slots);
	player->getSession()->send(packet);
}

auto InventoryPacket::sendHulkSmash(Player *player, int16_t slot, Item *hammered) -> void {
	vector_t<InventoryPacketOperation> ops;
	ops.emplace_back(OperationTypes::RemoveItem, hammered, slot);
	ops.emplace_back(OperationTypes::AddItem, hammered, slot);
	inventoryOperation(player, false, ops);
}

auto InventoryPacket::sendHammerUpdate(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_HAMMER);
	packet.add<int8_t>(0x38); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	player->getSession()->send(packet);
}

auto InventoryPacket::sendChalkboardUpdate(Player *player, const string_t &msg) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHALKBOARD);
	packet.add<int32_t>(player->getId());
	packet.add<bool>(!msg.empty());
	packet.addString(msg);
	player->getMap()->sendPacket(packet);
}

auto InventoryPacket::playCashSong(int32_t map, int32_t itemId, const string_t &playerName) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CASH_SONG);
	packet.add<int32_t>(itemId);
	packet.addString(playerName);
	Maps::getMap(map)->sendPacket(packet);
}

auto InventoryPacket::sendRewardItemAnimation(Player *player, int32_t itemId, const string_t &effect) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(0x0E);
	packet.add<int32_t>(itemId);
	packet.add<int8_t>(1); // Unk...?
	packet.addString(effect);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(0x0E);
	packet.add<int32_t>(itemId);
	packet.add<int8_t>(1); // Unk...?
	packet.addString(effect);
	player->getMap()->sendPacket(packet, player);
}

auto InventoryPacket::sendItemExpired(Player *player, const vector_t<int32_t> &items) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_NOTICE);
	packet.add<int8_t>(0x08);
	packet.add<uint8_t>(items.size());
	for (size_t i = 0; i < items.size(); ++i) {
		packet.add<int32_t>(items[i]);
	}
	player->getSession()->send(packet);
}