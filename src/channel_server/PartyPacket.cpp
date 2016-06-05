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
#include "PartyPacket.hpp"
#include "common/game_constants.hpp"
#include "common/inter_helper.hpp"
#include "common/session.hpp"
#include "common/wide_point.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/MysticDoor.hpp"
#include "channel_server/Party.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayerSkills.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace party {

PACKET_IMPL(error, int8_t error) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(error);
	return builder;
}

PACKET_IMPL(invite_error, int8_t error, const string &player) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(error)
		.add<string>(player);
	return builder;
}

PACKET_IMPL(custom_error, const string &error) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x24);

	if (error.empty()) {
		builder
			.add<bool>(true)
			.add<string>(error);
	}
	else {
		builder.add<bool>(false);
	}

	return builder;
}

PACKET_IMPL(create_party, vana::channel_server::party *party, ref_ptr<vana::channel_server::player> leader) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x08)
		.add<game_party_id>(party->get_id());

	if (ref_ptr<mystic_door> door = leader->get_skills()->get_mystic_door()) {
		if (door->get_map_id() == leader->get_map_id()) {
			builder
				.add<game_map_id>(door->get_map_id())
				.add<game_map_id>(door->get_town_id())
				.add<point>(door->get_map_pos());
		}
		else {
			builder
				.add<game_map_id>(door->get_town_id())
				.add<game_map_id>(door->get_map_id())
				.add<point>(door->get_town_pos());
		}
	}
	else {
		builder
			.add<game_map_id>(vana::maps::no_map)
			.add<game_map_id>(vana::maps::no_map)
			.add<point>(point{});
	}
	return builder;
}

PACKET_IMPL(join_party, game_map_id target_map_id, vana::channel_server::party *party, const string &player) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0F)
		.add<game_party_id>(party->get_id())
		.add<string>(player)
		.add_buffer(update_party(target_map_id, party));
	return builder;
}

PACKET_IMPL(leave_party, game_map_id target_map_id, vana::channel_server::party *party, game_player_id player_id, const string &name, bool kicked) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<game_party_id>(party->get_id())
		.add<game_player_id>(player_id)
		.add<bool>(true) // Is a regular leave (aka not disbanding)
		.add<bool>(kicked)
		.add<string>(name)
		.add_buffer(update_party(target_map_id, party));
	return builder;
}

PACKET_IMPL(invite_player, vana::channel_server::party *party, const string &inviter) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x04)
		.add<game_party_id>(party->get_id())
		.add<string>(inviter)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(disband_party, vana::channel_server::party *party) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<game_party_id>(party->get_id())
		.add<game_player_id>(party->get_leader_id())
		.add<bool>(false) // Is not a regular leave (aka disbanding)
		.add<game_party_id>(party->get_id());
	return builder;
}

PACKET_IMPL(set_leader, vana::channel_server::party *party, game_player_id new_leader) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x1B)
		.add<game_player_id>(new_leader)
		.add<bool>(false);
	return builder;
}

PACKET_IMPL(silent_update, game_map_id target_map_id, vana::channel_server::party *party) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x07)
		.add<game_party_id>(party->get_id())
		.add_buffer(update_party(target_map_id, party));
	return builder;
}

PACKET_IMPL(update_party, game_map_id target_map_id, vana::channel_server::party *party) {
	packet_builder builder;
	auto &members = party->get_members();
	size_t offset = parties::max_members - members.size();
	size_t i = 0;
	game_channel_id channel_id = channel_server::get_instance().get_channel_id();
	auto &provider = channel_server::get_instance().get_player_data_provider();

	// Add party member IDs to packet
	for (const auto &kvp : members) {
		builder.add<game_player_id>(kvp.first);
	}
	for (i = 0; i < offset; i++) {
		builder.add<game_player_id>(0);
	}

	// Add party member names to packet
	for (const auto &kvp : members) {
		auto player = provider.get_player_data(kvp.first);
		builder.add<string>(player->name, 13);
	}
	for (i = 0; i < offset; i++) {
		builder.add<string>("", 13);
	}

	// Add party member jobs to packet
	for (const auto &kvp : members) {
		auto player = provider.get_player_data(kvp.first);
		builder.add<int32_t>(player->job.get(-1));
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member levels to packet
	for (const auto &kvp : members) {
		auto player = provider.get_player_data(kvp.first);
		builder.add<int32_t>(player->level.is_initialized() ?
			player->level.get() :
			-1);
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member channels to packet
	for (const auto &kvp : members) {
		auto player = provider.get_player_data(kvp.first);
		builder.add<int32_t>(player->cash_shop ?
			-1 :
			player->channel.get(-2));
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(-2);
	}

	builder.add<game_player_id>(party->get_leader_id());

	// Add party member maps to packet
	for (const auto &kvp : members) {
		auto player = provider.get_player_data(kvp.first);
		if (!player->cash_shop && !player->mts && player->channel == channel_id && player->map == target_map_id) {
			builder.add<game_map_id>(target_map_id);
		}
		else {
			builder.add<game_map_id>(0);
		}
	}
	for (i = 0; i < offset; i++) {
		builder.add<game_map_id>(-2);
	}

	// Mystic Door information
	for (const auto &kvp : members) {
		if (kvp.second == nullptr) {
			builder
				.add<game_map_id>(vana::maps::no_map)
				.add<game_map_id>(vana::maps::no_map)
				.add<wide_point>(wide_point{-1, -1});
		}
		else if (ref_ptr<mystic_door> door = kvp.second->get_skills()->get_mystic_door()) {
			builder
				.add<game_map_id>(door->get_town_id())
				.add<game_map_id>(door->get_map_id())
				.add<wide_point>(wide_point{door->get_map_pos()});
		}
		else {
			builder
				.add<game_map_id>(vana::maps::no_map)
				.add<game_map_id>(vana::maps::no_map)
				.add<wide_point>(wide_point{-1, -1});
		}
	}
	for (i = 0; i < offset; i++) {
		builder
			.add<game_map_id>(vana::maps::no_map)
			.add<game_map_id>(vana::maps::no_map)
			.add<wide_point>(wide_point{-1, -1});
	}
	return builder;
}

PACKET_IMPL(update_door, uint8_t zero_based_player_index, ref_ptr<mystic_door> door) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x25)
		.add<uint8_t>(zero_based_player_index);

	if (door == nullptr) {
		builder
			.add<game_map_id>(vana::maps::no_map)
			.add<game_map_id>(vana::maps::no_map)
			.add<wide_point>(wide_point{-1, -1});
	}
	else {
		builder
			.add<game_map_id>(door->get_town_id())
			.add<game_map_id>(door->get_map_id())
			.add<wide_point>(wide_point{door->get_map_pos()});
	}

	return builder;
}

}
}
}
}