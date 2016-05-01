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
#include "InventoryPacket.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/InterHeader.hpp"
#include "Common/ItemConstants.hpp"
#include "Common/Session.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/InventoryPacketHelper.hpp"
#include "ChannelServer/Map.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayerInventory.hpp"
#include "ChannelServer/PlayerPacketHelper.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Inventory {

SPLIT_PACKET_IMPL(updatePlayer, ref_ptr_t<Player> player) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PLAYER_CHANGE_LOOK)
		.add<player_id_t>(player->getId())
		.add<int8_t>(1)
		.addBuffer(Helpers::addPlayerDisplay(player))
		.unk<int8_t>()
		.unk<int16_t>();

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(sitChair, player_id_t playerId, item_id_t chairId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int16_t>(1)
		.unk<int32_t>();

	builder.map
		.add<header_t>(SMSG_CHAIR_SIT)
		.add<player_id_t>(playerId)
		.add<item_id_t>(chairId);
	return builder;
}

SPLIT_PACKET_IMPL(stopChair, player_id_t playerId, bool seatTaken) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_CHAIR)
		.add<int8_t>(0);

	if (seatTaken) {
		return builder;
	}

	builder.map
		.add<header_t>(SMSG_CHAIR_SIT)
		.add<player_id_t>(playerId)
		.add<item_id_t>(0);
	return builder;
}

SPLIT_PACKET_IMPL(useScroll, player_id_t playerId, int8_t succeed, bool destroy, bool legendarySpirit) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SCROLL_USE)
		.add<player_id_t>(playerId)
		.add<int8_t>(succeed)
		.add<bool>(destroy)
		.add<int16_t>(legendarySpirit);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(sendChalkboardUpdate, player_id_t playerId, const string_t &msg) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_CHALKBOARD)
		.add<player_id_t>(playerId)
		.add<bool>(!msg.empty())
		.add<string_t>(msg);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(useSkillbook, player_id_t playerId, skill_id_t skillId, int32_t newMaxLevel, bool use, bool succeed) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILLBOOK)
		.add<player_id_t>(playerId)
		.add<int8_t>(1) // Number of skills? Maybe just padding or random boolean
		.add<skill_id_t>(skillId)
		.add<int32_t>(newMaxLevel)
		.add<bool>(use)
		.add<bool>(succeed);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(useItemEffect, player_id_t playerId, item_id_t itemId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_ITEM_EFFECT)
		.add<player_id_t>(playerId)
		.add<item_id_t>(itemId);

	builder.map.addBuffer(builder.player);
	return builder;
}

PACKET_IMPL(inventoryOperation, bool unk, const vector_t<InventoryPacketOperation> &operations) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_OPERATION)
		.add<bool>(unk)
		.add<uint8_t>(static_cast<uint8_t>(operations.size()));

	int8_t addedByte = -1;

	for (const auto &operation : operations) {
		builder
			.add<int8_t>(operation.operationType)
			.add<inventory_t>(GameLogicUtilities::getInventory(operation.item->getId()));

		switch (operation.operationType) {
			case Packets::Inventory::OperationTypes::AddItem:
				builder.addBuffer(Helpers::addItemInfo(operation.currentSlot, operation.item, true));
				break;
			case Packets::Inventory::OperationTypes::ModifyQuantity:
				builder
					.add<inventory_slot_t>(operation.currentSlot)
					.add<slot_qty_t>(operation.item->getAmount());
				break;
			case Packets::Inventory::OperationTypes::ModifySlot:
				builder
					.add<inventory_slot_t>(operation.oldSlot)
					.add<inventory_slot_t>(operation.currentSlot);

				if (addedByte == -1) {
					if (operation.oldSlot < 0) {
						addedByte = 1;
					}
					else if (operation.currentSlot < 0) {
						addedByte = 2;
					}
				}
				break;
			case Packets::Inventory::OperationTypes::RemoveItem:
				builder.add<inventory_slot_t>(operation.currentSlot);
				break;
		}
	}

	if (addedByte != -1) {
		builder.add<int8_t>(addedByte);
	}
	return builder;
}

PACKET_IMPL(sitMapChair, seat_id_t chairId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHAIR)
		.add<int8_t>(1)
		.add<seat_id_t>(chairId);
	return builder;
}

PACKET_IMPL(showMegaphone, const string_t &msg) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(2)
		.add<string_t>(msg);
	return builder;
}

PACKET_IMPL(showSuperMegaphone, const string_t &msg, bool whisper) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(3)
		.add<string_t>(msg)
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);
	return builder;
}

PACKET_IMPL(showMessenger, const string_t &playerName, const string_t &msg1, const string_t &msg2, const string_t &msg3, const string_t &msg4, unsigned char *displayInfo, int32_t displayInfoSize, item_id_t itemId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_AVATAR_MEGAPHONE)
		.add<item_id_t>(itemId)
		.add<string_t>(playerName)
		.add<string_t>(msg1)
		.add<string_t>(msg2)
		.add<string_t>(msg3)
		.add<string_t>(msg4)
		.add<int32_t>(ChannelServer::getInstance().getChannelId())
		.addBuffer(displayInfo, displayInfoSize);
	return builder;
}

PACKET_IMPL(showItemMegaphone, const string_t &msg, bool whisper, Item *item) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(8)
		.add<string_t>(msg)
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);

	if (item == nullptr) {
		builder.add<int8_t>(0);
	}
	else {
		builder.addBuffer(Helpers::addItemInfo(1, item));
	}
	return builder;
}

PACKET_IMPL(showTripleMegaphone, int8_t lines, const string_t &line1, const string_t &line2, const string_t &line3, bool whisper) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(0x0a)
		.add<string_t>(line1)
		.add<int8_t>(lines);
	if (lines > 1) {
		builder.add<string_t>(line2);
	}
	if (lines > 2) {
		builder.add<string_t>(line3);
	}
	builder
		.add<int8_t>(ChannelServer::getInstance().getChannelId())
		.add<bool>(whisper);
	return builder;
}

PACKET_IMPL(updateSlots, inventory_t inventory, inventory_slot_count_t slots) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_SLOT_UPDATE)
		.add<inventory_t>(inventory)
		.add<inventory_slot_count_t>(slots);
	return builder;
}

PACKET_IMPL(blankUpdate) {
	PacketBuilder builder;
	vector_t<InventoryPacketOperation> ops;
	builder.addBuffer(inventoryOperation(true, ops));
	return builder;
}

PACKET_IMPL(sendRockUpdate, int8_t mode, int8_t type, const vector_t<map_id_t> &maps) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_TELEPORT_ROCK)
		.add<int8_t>(mode)
		.add<int8_t>(type)
		.addBuffer(Helpers::fillRockPacket(maps, (type == RockTypes::Regular ? Inventories::TeleportRockMax : Inventories::VipRockMax)));
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

PACKET_IMPL(sendMesobagSucceed, mesos_t mesos) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESOBAG_SUCCESS)
		.add<mesos_t>(mesos);
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

PACKET_IMPL(sendHulkSmash, inventory_slot_t slot, Item *hammered) {
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

PACKET_IMPL(playCashSong, item_id_t itemId, const string_t &playerName) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CASH_SONG)
		.add<item_id_t>(itemId)
		.add<string_t>(playerName);
	return builder;
}

SPLIT_PACKET_IMPL(sendRewardItemAnimation, player_id_t playerId, item_id_t itemId, const string_t &effect) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(0x0E)
		.add<item_id_t>(itemId)
		.add<int8_t>(1) // Unk...?
		.add<string_t>(effect);

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.addBuffer(packet);
	return builder;
}

PACKET_IMPL(sendItemExpired, const vector_t<item_id_t> &items) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_NOTICE)
		.add<int8_t>(0x08)
		.add<uint8_t>(static_cast<uint8_t>(items.size()));

	for (const auto &itemId : items) {
		builder.add<item_id_t>(itemId);
	}
	return builder;
}

}
}
}
}