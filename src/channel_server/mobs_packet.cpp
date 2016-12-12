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
#include "mobs_packet.hpp"
#include "common/session.hpp"
#include "common/wide_point.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"
#include "channel_server/status_info.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace mobs {

PACKET_IMPL(spawn_mob, ref_ptr<mob> value, int8_t summon_effect, ref_ptr<mob> owner, mob_spawn_type spawn) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_SHOW)
		.add_buffer(mob_packet(value, summon_effect, owner, spawn));
	return builder;
}

PACKET_IMPL(request_control, ref_ptr<mob> value, mob_spawn_type spawn) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_CONTROL)
		.unk<int8_t>(1)
		.add_buffer(mob_packet(value, 0, nullptr, spawn));
	return builder;
}

PACKET_IMPL(mob_packet, ref_ptr<mob> value, int8_t summon_effect, ref_ptr<mob> owner, mob_spawn_type spawn) {
	packet_builder builder;
	builder
		.add<game_map_object>(value->get_map_mob_id())
		.add<int8_t>(static_cast<int8_t>(value->get_control_status()))
		.add<game_mob_id>(value->get_mob_id())
		.add<int32_t>(value->get_status_bits());

	for (const auto &kvp : value->get_status_info()) {
		if (kvp.first != constant::status_effect::mob::empty) {
			const status_info &info = kvp.second;
			builder.add<int16_t>(static_cast<int16_t>(info.val));
			if (info.skill_id >= 0) {
				builder.add<int32_t>(info.skill_id);
			}
			else {
				builder
					.add<int16_t>(info.mob_skill)
					.add<int16_t>(info.level);
			}
			builder.add<int16_t>(1);
		}
		else {
			builder.unk<int32_t>();
		}
	}

	builder.add<point>(value->get_pos());

	int8_t bitfield = (owner != nullptr ? 0x08 : 0x02) | (value->is_facing_left() ? 0x01 : 0);
	if (value->can_fly()) {
		bitfield |= 0x04;
	}

	builder
		.add<int8_t>(bitfield) // 0x08 - a summon, 0x04 - flying, 0x02 - ???, 0x01 - faces left
		.add<game_foothold_id>(value->get_foothold())
		.add<game_foothold_id>(value->get_origin_foothold());

	if (owner != nullptr) {
		builder
			.add<int8_t>(summon_effect != 0 ? summon_effect : -3)
			.add<game_map_object>(owner->get_map_mob_id());
	}
	else {
		builder.add<int8_t>(spawn == mob_spawn_type::spawn ? -2 : -1);
	}

	builder
		.add<int8_t>(-1) // Monster Carnival team
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(end_control_mob, game_map_object map_mob_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_CONTROL)
		.add<int8_t>(0)
		.add<game_map_object>(map_mob_id);
	return builder;
}

PACKET_IMPL(move_mob_response, game_map_object map_mob_id, int16_t move_id, bool skill_possible, int32_t mp, game_mob_skill_id skill, game_mob_skill_level level) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_MOVEMENT)
		.add<game_map_object>(map_mob_id)
		.add<int16_t>(move_id)
		.add<bool>(skill_possible)
		.add<int16_t>(static_cast<int16_t>(mp))
		.add<game_mob_skill_id>(skill)
		.add<game_mob_skill_level>(level);
	return builder;
}

PACKET_IMPL(move_mob, game_map_object map_mob_id, bool skill_possible, int8_t raw_action, game_mob_skill_id skill, game_mob_skill_level level, int16_t option, const move_path &path) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_CONTROL_MOVEMENT)
		.add<game_map_object>(map_mob_id)
		.add<bool>(skill_possible)
		.add<int8_t>(raw_action)
		.add<game_mob_skill_id>(skill)
		.add<game_mob_skill_level>(level)
		.add<int16_t>(option);

	path.write_to_packet(builder);
	return builder;
}

PACKET_IMPL(heal_mob, game_map_object map_mob_id, int32_t amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_DAMAGE)
		.add<game_map_object>(map_mob_id)
		.unk<int8_t>()
		.add<int32_t>(-amount)
		.unk<int8_t>()
		.unk<int8_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(hurt_mob, game_map_object map_mob_id, game_damage amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_DAMAGE)
		.add<game_map_object>(map_mob_id)
		.unk<int8_t>()
		.add<game_damage>(amount)
		.unk<int8_t>()
		.unk<int8_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(damage_friendly_mob, ref_ptr<mob> value, game_damage damage) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_DAMAGE)
		.add<game_map_object>(value->get_map_mob_id())
		.add<int8_t>(1)
		.add<game_damage>(damage)
		.add<int32_t>(value->get_hp())
		.add<int32_t>(value->get_max_hp());
	return builder;
}

PACKET_IMPL(apply_status, game_map_object map_mob_id, int32_t status_mask, const vector<status_info> &info, int16_t delay, const vector<int32_t> &reflection) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_STATUS_ADDITION)
		.add<game_map_object>(map_mob_id)
		.add<int32_t>(status_mask);

	for (const auto &status : info) {
		builder.add<int16_t>(static_cast<int16_t>(status.val));
		if (status.skill_id >= 0) {
			builder.add<int32_t>(status.skill_id);
		}
		else {
			builder
				.add<int16_t>(status.mob_skill)
				.add<int16_t>(status.level);
		}
		builder.add<int16_t>(-1); // Not sure what this is
	}

	for (const auto &reflect : reflection) {
		builder.add<int32_t>(reflect);
	}

	builder.add<int16_t>(delay);

	const auto abc = unsigned long long(2);
	uint8_t buff_count = static_cast<uint8_t>(info.size());
	if (reflection.size() > 0) {
		buff_count /= 2; // This gives 2 buffs per reflection but it's really one buff
	}
	builder.add<uint8_t>(buff_count);
	return builder;
}

PACKET_IMPL(remove_status, game_map_object map_mob_id, int32_t status) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_STATUS_REMOVE)
		.add<game_map_object>(map_mob_id)
		.add<int32_t>(status)
		.add<int8_t>(1);
	return builder;
}

PACKET_IMPL(show_hp, game_map_object map_mob_id, int8_t percentage) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_HP_DISPLAY)
		.add<game_map_object>(map_mob_id)
		.add<int8_t>(percentage);
	return builder;
}

PACKET_IMPL(show_boss_hp, ref_ptr<mob> value) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x05)
		.add<game_map_object>(value->get_mob_id())
		.add<int32_t>(value->get_hp())
		.add<int32_t>(value->get_max_hp())
		.add<int8_t>(value->get_hp_bar_color())
		.add<int8_t>(value->get_hp_bar_bg_color());
	return builder;
}

PACKET_IMPL(die_mob, game_map_object map_mob_id, int8_t death) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MOB_DEATH)
		.add<game_map_object>(map_mob_id)
		.add<int8_t>(death);
	return builder;
}

PACKET_IMPL(show_spawn_effect, int8_t summon_effect, const point &pos) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x00)
		.add<int8_t>(summon_effect)
		.add<wide_point>(wide_point{pos});
	return builder;
}

}
}
}
}