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
#include "NpcPacket.hpp"
#include "ItemDataProvider.hpp"
#include "Maps.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "ShopDataProvider.hpp"
#include "SmsgHeader.hpp"

namespace NpcPacket {

PACKET_IMPL(showNpc, const NpcSpawnInfo &npc, int32_t id, bool show) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_SHOW)
		.add<int32_t>(id)
		.add<int32_t>(npc.id)
		.add<Pos>(npc.pos)
		.add<bool>(!npc.facesLeft)
		.add<int16_t>(npc.foothold)
		.add<int16_t>(npc.rx0)
		.add<int16_t>(npc.rx1)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(controlNpc, const NpcSpawnInfo &npc, int32_t id, bool show) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_CONTROL)
		.add<int8_t>(1)
		.add<int32_t>(id)
		.add<int32_t>(npc.id)
		.add<Pos>(npc.pos)
		.add<bool>(!npc.facesLeft)
		.add<int16_t>(npc.foothold)
		.add<int16_t>(npc.rx0)
		.add<int16_t>(npc.rx1)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(animateNpc, PacketReader &reader) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_NPC_ANIMATE);

	size_t len = reader.getBufferLength();
	if (len == 6) {
		// NPC talking
		builder
			.add<int32_t>(reader.get<int32_t>())
			.add<int16_t>(reader.get<int16_t>());
	}
	else if (len > 6) {
		// NPC moving
		builder.addBuffer(reader.getBuffer(), len - 9);
	}
	return builder;
}

PACKET_IMPL(showNpcEffect, int32_t index, bool show) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_SHOW_EFFECT)
		.add<int32_t>(index)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(bought, uint8_t msg) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_ITEM_PURCHASED)
		.add<int8_t>(msg);
	return builder;
}

PACKET_IMPL(showShop, const BuiltShopInfo &shop, int16_t rechargeableBonus) {
	PacketBuilder builder;
	int16_t shopCount = static_cast<int16_t>(shop.items.size() + shop.rechargeables.size());
	hash_set_t<int32_t> itemsAdded;

	builder
		.add<header_t>(SMSG_SHOP)
		.add<int32_t>(shop.npc)
		.add<int16_t>(0); // To be set later

	// Items
	for (const auto &item : shop.items) {
		builder
			.add<int32_t>(item->itemId)
			.add<int32_t>(item->price);

		if (GameLogicUtilities::isRechargeable(item->itemId)) {
			itemsAdded.emplace(item->itemId);
			double cost = 0.0;
			if (shop.rechargeables.size() > 0) {
				shopCount--;
				auto kvp = shop.rechargeables.find(item->itemId);
				if (kvp != std::end(shop.rechargeables)) {
					cost = kvp->second;
				}
			}
			builder.add<double>(cost);
		}
		else {
			builder.add<int16_t>(item->quantity);
		}
		auto itemInfo = ItemDataProvider::getInstance().getItemInfo(item->itemId);
		uint16_t maxSlot = itemInfo->maxSlot;
		if (GameLogicUtilities::isRechargeable(item->itemId)) {
			maxSlot += rechargeableBonus;
		}
		builder.add<int16_t>(maxSlot);
	}

	// Rechargeables
	for (const auto &kvp : shop.rechargeables) {
		if (itemsAdded.find(kvp.first) == std::end(itemsAdded)) {
			builder
				.add<int32_t>(kvp.first)
				.add<int32_t>(0)
				.add<double>(kvp.second)
				.add<int16_t>(ItemDataProvider::getInstance().getItemInfo(kvp.first)->maxSlot + rechargeableBonus);
		}
	}

	builder.set<int16_t>(shopCount, 6);
	return builder;
}

PACKET_IMPL(npcChat, int8_t type, int32_t npcId, const string_t &text, bool excludeText) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_TALK)
		.add<int8_t>(4)
		.add<int32_t>(npcId)
		.add<int8_t>(type);

	if (!excludeText) {
		builder.add<string_t>(text);
	}
	return builder;
}

}