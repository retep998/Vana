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
#include "map_packet.hpp"
#include "common/constant/map.hpp"
#include "common/constant/ship_kind.hpp"
#include "common/file_time.hpp"
#include "common/point.hpp"
#include "common/session.hpp"
#include "common/time_utilities.hpp"
#include "common/wide_point.hpp"
#include "channel_server/buffs.hpp"
#include "channel_server/buffs_packet_helper.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mist.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet_helper.hpp"
#include "channel_server/smsg_header.hpp"
#include <unordered_map>

namespace vana {
namespace channel_server {
namespace packets {
namespace map {

PACKET_IMPL(player_packet, ref_ptr<vana::channel_server::player> player) {
	packet_builder builder;

	builder
		.add<packet_header>(SMSG_MAP_PLAYER_SPAWN)
		.add<game_player_id>(player->get_id())
		.add<string>(player->get_name())
		.add<string>("") // Guild
		.add<int16_t>(0) // Guild icon garbage
		.add<int8_t>(0) // Guild icon garbage
		.add<int16_t>(0) // Guild icon garbage
		.add<int8_t>(0); // Guild icon garbage

	packet_builder ref = helpers::add_buff_map_values(player->get_active_buffs()->get_map_buff_values());
	builder.add_buffer(ref);

	builder
		.add<game_job_id>(player->get_stats()->get_job())
		.add_buffer(helpers::add_player_display(player))
		.unk<int32_t>()
		.add<game_item_id>(player->get_item_effect())
		.add<game_item_id>(player->get_chair())
		.add<point>(player->get_pos())
		.add<int8_t>(player->get_stance())
		.add<game_foothold_id>(player->get_foothold())
		.unk<int8_t>();

	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = player->get_pets()->get_summoned(i)) {
			builder
				.add<bool>(true)
				.add<game_item_id>(pet->get_item_id())
				.add<string>(pet->get_name())
				.add<game_pet_id>(pet->get_id())
				.add<point>(pet->get_pos())
				.add<int8_t>(pet->get_stance())
				.add<game_foothold_id>(pet->get_foothold())
				.add<bool>(pet->has_name_tag())
				.add<bool>(pet->has_quote_item());
		}
	}
	// End of pets
	builder.add<bool>(false);

	player->get_mounts()->mount_info_map_spawn_packet(builder);

	builder.add<int8_t>(0); // Player room

	bool has_chalkboard = !player->get_chalkboard().empty();
	builder.add<bool>(has_chalkboard);
	if (has_chalkboard) {
		builder.add<string>(player->get_chalkboard());
	}

	builder
		.add<int8_t>(0) // Rings (crush)
		.add<int8_t>(0) // Rings (friends)
		.add<int8_t>(0) // Ring (marriage)
		.unk<int8_t>()
		.unk<int8_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(remove_player, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_PLAYER_DESPAWN)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(change_map, ref_ptr<vana::channel_server::player> player, bool spawn_by_position, const point &spawn_position) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANGE_MAP)
		.add<int32_t>(channel_server::get_instance().get_channel_id())
		.add<uint8_t>(player->get_portal_count(true))
		.add<bool>(false) // Not a connect packet
		.add<int16_t>(0); // Some amount for a funny message at the top of the screen

	if (false) {
		size_t line_amount = 0;
		builder.add<string>("Message title");
		for (size_t i = 0; i < line_amount; i++) {
			builder.add<string>("Line");
		}
	}

	builder
		.add<game_map_id>(player->get_map_id())
		.add<game_portal_id>(player->get_map_pos())
		.add<game_health>(player->get_stats()->get_hp())
		.add<bool>(spawn_by_position);

	if (spawn_by_position) {
		builder.add<wide_point>(wide_point{spawn_position});
	}

	builder.add<file_time>(file_time{});
	return builder;
}

PACKET_IMPL(portal_blocked) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_UPDATE)
		.unk<int8_t>(1)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(show_clock, int8_t hour, int8_t min, int8_t sec) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_TIMER)
		.add<int8_t>(0x01)
		.add<int8_t>(hour)
		.add<int8_t>(min)
		.add<int8_t>(sec);
	return builder;
}

PACKET_IMPL(show_timer, const seconds &sec) {
	packet_builder builder;
	if (sec.count() > 0) {
		builder
			.add<packet_header>(SMSG_TIMER)
			.unk<int8_t>(0x02)
			.add<int32_t>(static_cast<int32_t>(sec.count()));
	}
	else {
		builder.add<packet_header>(SMSG_TIMER_OFF);
	}
	return builder;
}

PACKET_IMPL(force_map_equip) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_MAP_FORCE_EQUIPMENT);
	return builder;
}

PACKET_IMPL(show_event_instructions) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_EVENT_INSTRUCTION)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(spawn_mist, mist *mist, bool map_entry) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MIST_SPAWN)
		.add<game_mist_id>(mist->get_id())
		.add<int32_t>(mist->is_mob_mist() ? 0 : mist->is_poison() ? 1 : 2)
		.add<int32_t>(mist->get_owner_id())
		.add<game_skill_id>(mist->get_skill_id())
		.add<game_skill_level>(mist->get_skill_level())
		.add<int16_t>(map_entry ? 0 : mist->get_delay())
		.add<wide_point>(wide_point{mist->get_area().left_top()})
		.add<wide_point>(wide_point{mist->get_area().right_bottom()})
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(remove_mist, game_map_object id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MIST_DESPAWN)
		.add<game_map_object>(id);
	return builder;
}

PACKET_IMPL(spawn_door, ref_ptr<mystic_door> door, bool is_inside_town, bool already_open) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MYSTIC_DOOR_SPAWN)
		.add<bool>(already_open)
		.add<game_player_id>(door->get_owner_id())
		.add<point>(is_inside_town ? door->get_town_pos() : door->get_map_pos());
	return builder;
}

PACKET_IMPL(remove_door, ref_ptr<mystic_door> door, bool is_fade) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MYSTIC_DOOR_DESPAWN)
		.add<bool>(!is_fade)
		.add<game_player_id>(door->get_owner_id());
	return builder;
}

PACKET_IMPL(spawn_portal, ref_ptr<mystic_door> door, game_map_id calling_map) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_PORTAL_ACTION);
	if (door->get_map_id() == calling_map) {
		builder
			.add<game_map_id>(door->get_map_id())
			.add<game_map_id>(door->get_town_id())
			.add<point>(door->get_town_pos());
	}
	else {
		builder
			.add<game_map_id>(door->get_town_id())
			.add<game_map_id>(door->get_map_id())
			.add<point>(door->get_map_pos());
	}
	return builder;
}

PACKET_IMPL(remove_portal) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PORTAL_ACTION)
		.add<game_map_id>(constant::map::no_map)
		.add<game_map_id>(constant::map::no_map);
	return builder;
}

PACKET_IMPL(instant_warp, game_portal_id portal_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_TELEPORT)
		.add<int8_t>(0x01)
		.add<game_portal_id>(portal_id);
	return builder;
}

PACKET_IMPL(boat_dock_update, bool docked, int8_t ship_kind) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_SHIP)
		.add<int8_t>(ship_kind == constant::ship_kind::balrog ?
			0x0A :
			(docked ? 0x0C : 0x08))
		.add<int8_t>(ship_kind == constant::ship_kind::balrog ?
			(docked ? 0x04 : 0x05) :
			(docked ? 0x06 : 0x02));
	return builder;
}

PACKET_IMPL(change_weather, bool admin_weather, game_item_id item_id, const string &message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_WEATHER_EFFECT)
		.add<bool>(admin_weather)
		.add<game_item_id>(item_id);

	if (item_id != 0 && !admin_weather) {
		// Admin weathers doesn't have a message
		builder.add<string>(message);
	}
	return builder;
}

PACKET_IMPL(upgrade_tomb_effect, game_player_id player_id, game_item_id item_id, int32_t x, int32_t y) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_UPGRADE_TOMB_EFFECT)
		.add<game_player_id>(player_id)
		.add<game_item_id>(item_id)
		.add<int32_t>(x)
		.add<int32_t>(y);

	return builder;
}

}
}
}
}