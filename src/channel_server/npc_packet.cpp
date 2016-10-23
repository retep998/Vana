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
#include "npc_packet.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/shop.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace npc {

PACKET_IMPL(show_npc, const data::type::npc_spawn_info &npc, game_map_object id, bool show) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NPC_SHOW)
		.add<game_map_object>(id)
		.add<game_npc_id>(npc.id)
		.add<point>(npc.pos)
		.add<bool>(!npc.faces_left)
		.add<game_foothold_id>(npc.foothold)
		.add<game_coord>(npc.rx0)
		.add<game_coord>(npc.rx1)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(control_npc, const data::type::npc_spawn_info &npc, game_map_object id, bool show) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NPC_CONTROL)
		.add<int8_t>(1)
		.add<game_map_object>(id)
		.add<game_npc_id>(npc.id)
		.add<point>(npc.pos)
		.add<bool>(!npc.faces_left)
		.add<game_foothold_id>(npc.foothold)
		.add<game_coord>(npc.rx0)
		.add<game_coord>(npc.rx1)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(animate_npc, packet_reader &reader) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_NPC_ANIMATE);

	size_t len = reader.get_buffer_length();
	if (len == 6) {
		// NPC talking
		builder
			.add<int32_t>(reader.get<int32_t>())
			.add<int16_t>(reader.get<int16_t>());
	}
	else if (len > 6) {
		// NPC moving
		builder.add_buffer(reader.get_buffer(), len - 9);
	}
	return builder;
}

PACKET_IMPL(show_npc_effect, int32_t index, bool show) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NPC_SHOW_EFFECT)
		.add<int32_t>(index)
		.add<bool>(show);
	return builder;
}

PACKET_IMPL(bought, uint8_t msg) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_ITEM_PURCHASED)
		.add<int8_t>(msg);
	return builder;
}

PACKET_IMPL(show_shop, const shop_data &shop, game_slot_qty rechargeable_bonus) {
	packet_builder builder;
	game_slot_qty shop_count = static_cast<game_slot_qty>(shop.items.size() + shop.rechargeables.size());
	hash_set<game_item_id> items_added;

	builder
		.add<packet_header>(SMSG_SHOP)
		.add<game_npc_id>(shop.npc)
		.defer<game_slot_qty>();

	// Items
	for (const auto &item : shop.items) {
		builder
			.add<game_item_id>(item->item_id)
			.add<game_mesos>(item->price);

		if (game_logic_utilities::is_rechargeable(item->item_id)) {
			items_added.emplace(item->item_id);
			double cost = 0.0;
			if (shop.rechargeables.size() > 0) {
				shop_count--;
				auto kvp = shop.rechargeables.find(item->item_id);
				if (kvp != std::end(shop.rechargeables)) {
					cost = kvp->second;
				}
			}
			builder.add<double>(cost);
		}
		else {
			builder.add<game_slot_qty>(item->quantity);
		}
		auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item->item_id);
		game_slot_qty max_slot = item_info->max_slot;
		if (game_logic_utilities::is_rechargeable(item->item_id)) {
			max_slot += rechargeable_bonus;
		}
		builder.add<game_slot_qty>(max_slot);
	}

	// Rechargeables
	for (const auto &kvp : shop.rechargeables) {
		if (items_added.find(kvp.first) == std::end(items_added)) {
			builder
				.add<game_item_id>(kvp.first)
				.add<int32_t>(0)
				.add<double>(kvp.second)
				.add<game_slot_qty>(channel_server::get_instance().get_item_data_provider().get_item_info(kvp.first)->max_slot + rechargeable_bonus);
		}
	}

	builder.set<game_slot_qty>(shop_count, sizeof(packet_header) + sizeof(game_npc_id));
	return builder;
}

PACKET_IMPL(npc_chat, int8_t type, game_map_object npc_id, const string &text, bool exclude_text) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NPC_TALK)
		.add<int8_t>(4)
		.add<game_map_object>(npc_id)
		.add<int8_t>(type);

	if (!exclude_text) {
		builder.add<string>(text);
	}
	return builder;
}

auto npc_set_script(hash_map<int32_t, string> scripts) -> vector<packet_builder> {

	vector<packet_builder> output;
	
	packet_builder current_packet;
	uint8_t written_scripts = 0;

	for (auto &kvp : scripts) {
		if (written_scripts == 254) {
			current_packet.set<uint8_t>(written_scripts, 2);
			output.push_back(packet_builder{current_packet});
			current_packet = {};
			written_scripts = 0;
		}

		if (written_scripts == 0) {
			current_packet
				.add<packet_header>(SMSG_NPC_SET_SCRIPT)
				.add<uint8_t>(0);
		}
		
		current_packet
			.add<game_map_object>(kvp.first)
			.add<string>(kvp.second)
			.add<int32_t>(20010101) // Start and end date
			.add<int32_t>(20240101);

		written_scripts++;
	}

	if (written_scripts > 0) {		
		current_packet.set<uint8_t>(written_scripts, 2);
		output.push_back(packet_builder{current_packet});
	}

	return output;
}

}
}
}
}