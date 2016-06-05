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
#include "SkillsPacket.hpp"
#include "common/charge_or_stationary_skill_data.hpp"
#include "common/game_constants.hpp"
#include "common/session_temp.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/Skills.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace skills {

PACKET_IMPL(add_skill, game_skill_id skill_id, const player_skill_info &skill_info) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_SKILL_ADD)
		.add<int8_t>(1)
		.add<int16_t>(1)
		.add<game_skill_id>(skill_id)
		.add<int32_t>(skill_info.level)
		.add<int32_t>(skill_info.player_max_skill_level)
		.add<int8_t>(1);
	return builder;
}

SPLIT_PACKET_IMPL(show_skill, game_player_id player_id, game_skill_id skill_id, game_skill_level level, uint8_t direction, bool party, bool self) {
	split_packet_builder builder;
	packet_builder buffer;
	buffer
		.add<int8_t>(party ? 2 : 1)
		.add<game_skill_id>(skill_id)
		.add<game_skill_level>(level);

	switch (skill_id) {
		case vana::skills::hero::monster_magnet:
		case vana::skills::paladin::monster_magnet:
		case vana::skills::dark_knight::monster_magnet:
			buffer.add<uint8_t>(direction);
			break;
	}

	if (self) {
		if (party) {
			builder.player.add<packet_header>(SMSG_THEATRICS);
		}
		else {
			builder.player
				.add<packet_header>(SMSG_SKILL_SHOW)
				.add<game_player_id>(player_id);
		}
		builder.player.add_buffer(buffer);
	}
	else {
		builder.map
			.add<packet_header>(SMSG_SKILL_SHOW)
			.add<game_player_id>(player_id)
			.add_buffer(buffer);
	}
	return builder;
}

PACKET_IMPL(heal_hp, game_health hp) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(0x0A)
		.add<game_health>(hp);
	return builder;
}

SPLIT_PACKET_IMPL(show_skill_effect, game_player_id player_id, game_skill_id skill_id) {
	split_packet_builder builder;
	packet_builder buffer;
	switch (skill_id) {
		case vana::skills::fp_wizard::mp_eater:
		case vana::skills::il_wizard::mp_eater:
		case vana::skills::cleric::mp_eater:
			buffer
				.add<int8_t>(1)
				.add<game_skill_id>(skill_id)
				.add<int8_t>(1);
			break;
		case vana::skills::chief_bandit::meso_guard:
		case vana::skills::dragon_knight::dragon_blood:
			buffer
				.add<int8_t>(5)
				.add<game_skill_id>(skill_id);
			break;
		default:
			return builder;
	}

	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add_buffer(buffer);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add_buffer(buffer);
	return builder;
}

SPLIT_PACKET_IMPL(show_charge_or_stationary_skill, game_player_id player_id, const charge_or_stationary_skill_data &info) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_CHARGE_OR_STATIONARY_SKILL)
		.add<game_player_id>(player_id)
		.add<game_skill_id>(info.skill_id)
		.add<game_skill_level>(info.level)
		.add<int8_t>(info.direction)
		.add<int8_t>(info.weapon_speed);
	return builder;
}

SPLIT_PACKET_IMPL(end_charge_or_stationary_skill, game_player_id player_id, const charge_or_stationary_skill_data &info) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_CHARGE_OR_STATIONARY_SKILL_END)
		.add<game_player_id>(player_id)
		.add<game_skill_id>(info.skill_id);
	return builder;
}

SPLIT_PACKET_IMPL(show_magnet_success, game_map_object map_mob_id, uint8_t success) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_MOB_DRAGGED)
		.add<game_map_object>(map_mob_id)
		.add<uint8_t>(success);
	return builder;
}

PACKET_IMPL(send_cooldown, game_skill_id skill_id, seconds time) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_SKILL_COOLDOWN)
		.add<game_skill_id>(skill_id)
		.add<int16_t>(static_cast<int16_t>(time.count()));
	return builder;
}

SPLIT_PACKET_IMPL(show_berserk, game_player_id player_id, game_skill_level level, bool on) {
	split_packet_builder builder;
	packet_builder buffer;
	buffer
		.add<int8_t>(1)
		.add<game_skill_id>(vana::skills::dark_knight::berserk)
		.add<game_skill_level>(level)
		.add<bool>(on);

	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add_buffer(buffer);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add_buffer(buffer);
	return builder;
}

}
}
}
}