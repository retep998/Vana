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
#include "PlayerPacket.hpp"
#include "common/client_ip.hpp"
#include "common/file_time.hpp"
#include "common/inter_header.hpp"
#include "common/session.hpp"
#include "common/time_utilities.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/KeyMaps.hpp"
#include "channel_server/Pet.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayerPacketHelper.hpp"
#include "channel_server/SkillMacros.hpp"
#include "channel_server/Skills.hpp"
#include "channel_server/SmsgHeader.hpp"
#include <unordered_map>

namespace vana {
namespace channel_server {
namespace packets {
namespace player {

PACKET_IMPL(connect_data, ref_ptr<vana::channel_server::player> player) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANGE_MAP)
		.add<int32_t>(channel_server::get_instance().get_channel_id())
		.add<uint8_t>(player->get_portal_count(true))
		.add<bool>(true) // Is a connect packet
		.add<int16_t>(0); // Some amount for a funny message at the top of the screen

	if (false) {
		size_t line_amount = 0;
		builder.add<string>("Message title");
		for (size_t i = 0; i < line_amount; i++) {
			builder.add<string>("Line");
		}
	}

	player->initialize_rng(builder);

	builder
		.add<int64_t>(-1) // Refers to packet sections that are included in here - all of them
		.add<game_player_id>(player->get_id())
		.add<string>(player->get_name(), 13)
		.add<game_gender_id>(player->get_gender())
		.add<game_skin_id>(player->get_skin())
		.add<game_face_id>(player->get_face())
		.add<game_hair_id>(player->get_hair());

	player->get_pets()->connect_packet(builder);
	player->get_stats()->connect_packet(builder);

	builder
		.add<int32_t>(0) // Gachapon EXP
		.add<game_map_id>(player->get_map_id())
		.add<game_portal_id>(player->get_map_pos())
		.unk<int32_t>() // Added in .62
		.add<uint8_t>(player->get_buddy_list_size());

	player->get_skills()->connect_packet_for_blessing(builder);
	player->get_inventory()->connect_packet(builder);
	player->get_skills()->connect_packet(builder);
	player->get_quests()->connect_packet(builder);

	// Must do significant testing on all of the following to verify
	builder
		.add<int16_t>(0) // I think this is the minigame record
		.add<int16_t>(0) // I think this is the couple ring record
		.add<int16_t>(0) // I think this is the friendship ring record
		.add<int16_t>(0); // I think this is the marriage ring record

	player->get_inventory()->rock_packet(builder); // Teleport Rock/VIP Rock maps
	player->get_monster_book()->connect_packet(builder);

	builder
		.unk<int16_t>() 
		.add<int16_t>(0) // Amount of party quests (or extended quests? Maybe related to Battleship?), for every quest: game_quest_id quest_id, string quest_data
		.unk<int16_t>()
		.add<file_time>(file_time{});
	return builder;
}

PACKET_IMPL(show_keys, key_maps *key_maps) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_KEYMAP)
		.add<int8_t>(0);

	for (size_t i = 0; i < key_maps::key_count; i++) {
		if (key_maps::key_map *key_map = key_maps->get_key_map(i)) {
			builder
				.add<int8_t>(static_cast<int8_t>(key_map->type))
				.add<int32_t>(key_map->action);
		}
		else {
			builder
				.add<int8_t>(0)
				.add<int32_t>(0);
		}
	}
	return builder;
}

PACKET_IMPL(show_skill_macros, skill_macros *macros) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MACRO_LIST)
		.add<int8_t>(macros->get_max() + 1);

	for (int8_t i = 0; i <= macros->get_max(); i++) {
		if (skill_macros::skill_macro *macro = macros->get_skill_macro(i)) {
			builder
				.add<string>(macro->name)
				.add<bool>(macro->shout)
				.add<int32_t>(macro->skill1)
				.add<int32_t>(macro->skill2)
				.add<int32_t>(macro->skill3);
		}
		else {
			builder
				.add<string>("")
				.add<bool>(false)
				.add<int32_t>(0)
				.add<int32_t>(0)
				.add<int32_t>(0);
		}
	}
	return builder;
}

PACKET_IMPL(update_stat, int32_t update_bits, int32_t value, bool item_response) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_UPDATE)
		.add<bool>(item_response)
		.add<int32_t>(update_bits);

	switch (update_bits) {
		// For now it only accepts updateBits as a single unit, might be a collection later
		case stats::pet:
		case stats::level:
		case stats::job:
		case stats::str:
		case stats::dex:
		case stats::intl:
		case stats::luk:
		case stats::hp:
		case stats::max_hp:
		case stats::mp:
		case stats::max_mp:
		case stats::ap:
		case stats::sp:
			builder.add<int16_t>(static_cast<int16_t>(value));
			break;
		case stats::skin:
		case stats::face:
		case stats::hair:
		case stats::exp:
		case stats::fame:
		case stats::mesos:
			builder.add<int32_t>(value);
			break;
	}
	builder.add<int32_t>(value);
	return builder;
}

PACKET_IMPL(change_channel, const ip &ip, connection_port port) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANNEL_CHANGE)
		.add<bool>(true)
		.add<client_ip>(client_ip{ip})
		.add<connection_port>(port);
	return builder;
}

PACKET_IMPL(show_message, const game_chat &msg, int8_t type) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE)
		.add<int8_t>(type)
		.add<game_chat>(msg);

	if (type == notice_types::blue) {
		builder.add<int32_t>(0);
	}
	return builder;
}

PACKET_IMPL(group_chat, const string &name, const game_chat &msg, int8_t type) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE_GROUP)
		.add<int8_t>(type)
		.add<string>(name)
		.add<game_chat>(msg);
	return builder;
}

PACKET_IMPL(instruction_bubble, const game_chat &msg, game_coord width, int16_t time, bool is_static, int32_t x, int32_t y) {
	packet_builder builder;

	if (width == -1) {
		width = static_cast<game_coord>(msg.size()) * 10;
		if (width < 40) {
			width = 40; // Anything lower crashes client/doesn't look good
		}
	}

	builder
		.add<packet_header>(SMSG_BUBBLE)
		.add<game_chat>(msg)
		.add<game_coord>(width)
		.add<int16_t>(time)
		.add<bool>(!is_static);

	if (is_static) {
		builder
			.add<int32_t>(x)
			.add<int32_t>(y);
	}
	return builder;
}

PACKET_IMPL(show_hp_bar, game_player_id player_id, int32_t hp, int32_t max_hp) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PARTY_HP_DISPLAY)
		.add<game_player_id>(player_id)
		.add<int32_t>(hp)
		.add<int32_t>(max_hp);
	return builder;
}

PACKET_IMPL(send_blocked_message, int8_t type) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANNEL_BLOCKED)
		.add<int8_t>(type);
	return builder;
}

PACKET_IMPL(send_yellow_message, const game_chat &msg) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_YELLOW_MESSAGE)
		.add<bool>(true)
		.add<game_chat>(msg);
	return builder;
}

}
}
}
}