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
#include "NpcPacket.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include "Common/ShopDataProvider.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Npc {

PACKET_IMPL(showNpc, const NpcSpawnInfo &npc, map_object_t id, bool show) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_SHOW)
		.add<map_object_t>(id)
		.add<npc_id_t>(npc.id)
		.add<Point>(npc.pos)
		.add<bool>(!npc.facesLeft)
		.add<foothold_id_t>(npc.foothold)
		.add<coord_t>(npc.rx0)
		.add<coord_t>(npc.rx1)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(controlNpc, const NpcSpawnInfo &npc, map_object_t id, bool show) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_CONTROL)
		.add<int8_t>(1)
		.add<map_object_t>(id)
		.add<npc_id_t>(npc.id)
		.add<Point>(npc.pos)
		.add<bool>(!npc.facesLeft)
		.add<foothold_id_t>(npc.foothold)
		.add<coord_t>(npc.rx0)
		.add<coord_t>(npc.rx1)
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

PACKET_IMPL(showShop, const ShopData &shop, slot_qty_t rechargeableBonus) {
	PacketBuilder builder;
	slot_qty_t shopCount = static_cast<slot_qty_t>(shop.items.size() + shop.rechargeables.size());
	hash_set_t<item_id_t> itemsAdded;

	builder
		.add<header_t>(SMSG_SHOP)
		.add<npc_id_t>(shop.npc)
		.defer<slot_qty_t>();

	// Items
	for (const auto &item : shop.items) {
		builder
			.add<item_id_t>(item->itemId)
			.add<mesos_t>(item->price);

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
			builder.add<slot_qty_t>(item->quantity);
		}
		auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(item->itemId);
		slot_qty_t maxSlot = itemInfo->maxSlot;
		if (GameLogicUtilities::isRechargeable(item->itemId)) {
			maxSlot += rechargeableBonus;
		}
		builder.add<slot_qty_t>(maxSlot);
	}

	// Rechargeables
	for (const auto &kvp : shop.rechargeables) {
		if (itemsAdded.find(kvp.first) == std::end(itemsAdded)) {
			builder
				.add<item_id_t>(kvp.first)
				.add<int32_t>(0)
				.add<double>(kvp.second)
				.add<slot_qty_t>(ChannelServer::getInstance().getItemDataProvider().getItemInfo(kvp.first)->maxSlot + rechargeableBonus);
		}
	}

	builder.set<slot_qty_t>(shopCount, sizeof(header_t) + sizeof(npc_id_t));
	return builder;
}

PACKET_IMPL(npcChat, int8_t type, map_object_t npcId, const string_t &text, bool excludeText) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NPC_TALK)
		.add<int8_t>(4)
		.add<map_object_t>(npcId)
		.add<int8_t>(type);

	if (!excludeText) {
		builder.add<string_t>(text);
	}
	return builder;
}

}
}
}
}