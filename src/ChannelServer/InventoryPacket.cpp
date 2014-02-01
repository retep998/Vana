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
#include "InventoryPacket.hpp"
#include "ChannelServer.hpp"
#include "GameLogicUtilities.hpp"
#include "InterHeader.hpp"
#include "Inventory.hpp"
#include "InventoryPacketHelper.hpp"
#include "ItemConstants.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerInventory.hpp"
#include "PlayerPacketHelper.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace InventoryPacket {

SPLIT_PACKET_IMPL(updatePlayer, Player *player) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PLAYER_CHANGE_LOOK)
		.add<int32_t>(player->getId())
		.add<int8_t>(1)
		.addBuffer(PlayerPacketHelper::addPlayerDisplay(player))
		.add<int8_t>(0)
		.add<int16_t>(0);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(sitChair, int32_t playerId, int32_t chairId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int16_t>(1)
		.add<int32_t>(0);

	builder.map
		.add<header_t>(SMSG_CHAIR_SIT)
		.add<int32_t>(playerId)
		.add<int32_t>(chairId);
	return builder;
}

SPLIT_PACKET_IMPL(stopChair, int32_t playerId, bool seatTaken) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_CHAIR)
		.add<int8_t>(0);
	if (seatTaken) {
		return builder;
	}

	builder.map
		.add<header_t>(SMSG_CHAIR_SIT)
		.add<int32_t>(playerId)
		.add<int32_t>(0);
	return builder;
}

SPLIT_PACKET_IMPL(useScroll, int32_t playerId, int8_t succeed, bool destroy, bool legendarySpirit) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SCROLL_USE)
		.add<int32_t>(playerId)
		.add<int8_t>(succeed)
		.add<bool>(destroy)
		.add<int16_t>(legendarySpirit);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(sendChalkboardUpdate, int32_t playerId, const string_t &msg) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_CHALKBOARD)
		.add<int32_t>(playerId)
		.add<bool>(!msg.empty())
		.addString(msg);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(useSkillbook, int32_t playerId, int32_t skillId, int32_t newMaxLevel, bool use, bool succeed) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILLBOOK)
		.add<int32_t>(playerId)
		.add<int8_t>(1) // Number of skills? Maybe just padding or random boolean
		.add<int32_t>(skillId)
		.add<int32_t>(newMaxLevel)
		.add<bool>(use)
		.add<bool>(succeed);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(useItemEffect, int32_t playerId, int32_t itemId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_ITEM_EFFECT)
		.add<int32_t>(playerId)
		.add<int32_t>(itemId);

	builder.map.addBuffer(builder.player);
	return builder;
}

PACKET_IMPL(inventoryOperation, bool unk, const vector_t<InventoryPacketOperation> &operations) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_OPERATION)
		.add<bool>(unk)
		.add<uint8_t>(operations.size());

	for (const auto &operation : operations) {
		builder
			.add<int8_t>(operation.operationType)
			.add<int8_t>(GameLogicUtilities::getInventory(operation.item->getId()));

		switch (operation.operationType) {
			case InventoryPacket::OperationTypes::AddItem:
				builder.addBuffer(PlayerPacketHelper::addItemInfo(operation.currentSlot, operation.item, true));
				break;
			case InventoryPacket::OperationTypes::ModifyQuantity:
				builder
					.add<int16_t>(operation.currentSlot)
					.add<int16_t>(operation.item->getAmount());
				break;
			case InventoryPacket::OperationTypes::ModifySlot:
				builder
					.add<int16_t>(operation.oldSlot)
					.add<int16_t>(operation.currentSlot);
				if (operation.oldSlot < 0) {
					builder.add<int8_t>(1);
				}
				else if (operation.currentSlot < 0) {
					builder.add<int8_t>(2);
				}
				break;
			case InventoryPacket::OperationTypes::RemoveItem:
				builder.add<int16_t>(operation.currentSlot);
				break;
		}
	}
	return builder;
}

PACKET_IMPL(sitMapChair, int16_t chairId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHAIR)
		.add<int8_t>(1)
		.add<int16_t>(chairId);
	return builder;
}

PACKET_IMPL(showMegaphone, const string_t &msg) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(2)
		.addString(msg);
	return builder;
}

PACKET_IMPL(showSuperMegaphone, const string_t &msg, bool whisper) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(3)
		.addString(msg)
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);
	return builder;
}

PACKET_IMPL(showMessenger, const string_t &playerName, const string_t &msg1, const string_t &msg2, const string_t &msg3, const string_t &msg4, unsigned char *displayInfo, int32_t displayInfoSize, int32_t itemId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_AVATAR_MEGAPHONE)
		.add<int32_t>(itemId)
		.addString(playerName)
		.addString(msg1)
		.addString(msg2)
		.addString(msg3)
		.addString(msg4)
		.add<int32_t>(ChannelServer::getInstance().getChannelId())
		.addBuffer(displayInfo, displayInfoSize);
	return builder;
}

PACKET_IMPL(showItemMegaphone, const string_t &msg, bool whisper, Item *item) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(8)
		.addString(msg)
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);

	if (item == nullptr) {
		builder.add<int8_t>(0);
	}
	else {
		builder.addBuffer(PlayerPacketHelper::addItemInfo(1, item));
	}
	return builder;
}

PACKET_IMPL(showTripleMegaphone, int8_t lines, const string_t &line1, const string_t &line2, const string_t &line3, bool whisper) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(0x0a)
		.addString(line1)
		.add<int8_t>(lines);
	if (lines > 1) {
		builder.addString(line2);
	}
	if (lines > 2) {
		builder.addString(line3);
	}
	builder
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);
	return builder;
}


PACKET_IMPL(updateSlots, int8_t inventory, int8_t slots) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_SLOT_UPDATE)
		.add<int8_t>(inventory)
		.add<int8_t>(slots);
	return builder;
}

PACKET_IMPL(blankUpdate) {
	PacketBuilder builder;
	vector_t<InventoryPacketOperation> ops;
	builder.addBuffer(inventoryOperation(true, ops));
	return builder;
}

PACKET_IMPL(sendRockUpdate, int8_t mode, int8_t type, const vector_t<int32_t> &maps) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_TELEPORT_ROCK)
		.add<int8_t>(mode)
		.add<int8_t>(type)
		.addBuffer(InventoryPacketHelper::fillRockPacket(maps, (type == RockTypes::Regular ? Inventories::TeleportRockMax : Inventories::VipRockMax)));
	return builder;
}

PACKET_IMPL(sendRockError, int8_t code, int8_t type) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_TELEPORT_ROCK)
		.add<int8_t>(code)
		.add<int8_t>(type);
	return builder;
}

PACKET_IMPL(sendMesobagSucceed, int32_t mesos) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESOBAG_SUCCESS)
		.add<int32_t>(mesos);
	return builder;
}

PACKET_IMPL(sendMesobagFailed) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_MESOBAG_FAILURE);
	return builder;
}

PACKET_IMPL(useCharm, uint8_t charmsLeft, uint8_t daysLeft) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(0x06)
		.add<int8_t>(0x01)
		.add<uint8_t>(charmsLeft)
		.add<uint8_t>(daysLeft);
	return builder;
}

PACKET_IMPL(sendHammerSlots, int32_t slots) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_HAMMER)
		.add<int8_t>(0x34) // No idea... mode of some sort, I think
		.add<int32_t>(0x00)
		.add<int32_t>(slots);
	return builder;
}

PACKET_IMPL(sendHulkSmash, int16_t slot, Item *hammered) {
	PacketBuilder builder;
	vector_t<InventoryPacketOperation> ops;
	ops.emplace_back(OperationTypes::RemoveItem, hammered, slot);
	ops.emplace_back(OperationTypes::AddItem, hammered, slot);
	builder.addBuffer(inventoryOperation(false, ops));
	return builder;
}

PACKET_IMPL(sendHammerUpdate) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_HAMMER)
		.add<int8_t>(0x38) // No idea... mode of some sort, I think
		.add<int32_t>(0x00);
	return builder;
}

PACKET_IMPL(playCashSong, int32_t itemId, const string_t &playerName) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CASH_SONG)
		.add<int32_t>(itemId)
		.addString(playerName);
	return builder;
}

SPLIT_PACKET_IMPL(sendRewardItemAnimation, int32_t playerId, int32_t itemId, const string_t &effect) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(0x0E)
		.add<int32_t>(itemId)
		.add<int8_t>(1) // Unk...?
		.addString(effect);

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<int32_t>(playerId)
		.addBuffer(packet);
	return builder;
}

PACKET_IMPL(sendItemExpired, const vector_t<int32_t> &items) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_NOTICE)
		.add<int8_t>(0x08)
		.add<uint8_t>(items.size());

	for (const auto &itemId : items) {
		builder.add<int32_t>(itemId);
	}
	return builder;
}

}