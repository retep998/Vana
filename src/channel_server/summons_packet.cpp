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
#include "summons_packet.hpp"
#include "common/game_logic_utilities.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"
#include "channel_server/summon.hpp"

namespace vana {
namespace channel_server {
namespace packets {

SPLIT_PACKET_IMPL(show_summon, game_player_id player_id, summon *summon, bool is_map_entry) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SUMMON_SPAWN)
		.add<game_player_id>(player_id)
		.add<game_summon_id>(summon->get_id())
		.add<game_skill_id>(summon->get_skill_id())
		.add<game_skill_level>(summon->get_skill_level())
		.add<point>(summon->get_pos())
		.add<int8_t>(summon->get_stance())
		.add<game_foothold_id>(summon->get_foothold())
		.add<int8_t>(summon->get_movement_type())
		.add<int8_t>(summon->get_action_type())
		.add<bool>(is_map_entry);

	builder.map.add_buffer(builder.player);
	return builder;
}

PACKET_IMPL(move_summon, game_player_id player_id, summon *summon, const move_path &path) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_SUMMON_MOVEMENT)
		.add<game_player_id>(player_id)
		.add<game_summon_id>(summon->get_id());

	path.write_to_packet(builder);
	return builder;
}

SPLIT_PACKET_IMPL(remove_summon, game_player_id player_id, summon *summon, int8_t message) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SUMMON_DESPAWN)
		.add<game_player_id>(player_id)
		.add<game_summon_id>(summon->get_id())
		.add<int8_t>(message);

	builder.map.add_buffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(damage_summon, game_player_id player_id, game_summon_id summon_id, int8_t unk, game_damage damage, game_map_object mob_id) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SUMMON_DAMAGE)
		.add<game_player_id>(player_id)
		.add<game_summon_id>(summon_id)
		.add<int8_t>(unk)
		.add<game_damage>(damage)
		.add<game_map_object>(mob_id)
		.unk<int8_t>();

	builder.map.add_buffer(builder.player);
	return builder;
}

PACKET_IMPL(summon_skill, game_player_id player_id, game_skill_id skill_id, uint8_t display, game_skill_level level) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_SUMMON_SKILL)
		.add<game_player_id>(player_id)
		.add<game_skill_id>(skill_id)
		.add<uint8_t>(display);
	return builder;
}

SPLIT_PACKET_IMPL(summon_skill_effect, game_player_id player_id, game_skill_id skill_id, uint8_t display, game_skill_level level) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(2)
		.add<game_skill_id>(skill_id)
		.add<game_skill_level>(level)
		.add<int8_t>(1);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add<int8_t>(2)
		.add<game_skill_id>(skill_id)
		.add<uint8_t>(display)
		.add<int8_t>(1);
	return builder;
}

}
}
}