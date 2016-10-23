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
#include "players_packet.hpp"
#include "common/attack_data.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/return_damage_data.hpp"
#include "common/session.hpp"
#include "common/wide_point.hpp"
#include "channel_server.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace players {

SPLIT_PACKET_IMPL(show_moving, game_player_id player_id, const move_path &path) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_PLAYER_MOVEMENT)
		.add<game_player_id>(player_id);
	path.write_to_packet(builder.map);
	return builder;
}

SPLIT_PACKET_IMPL(face_expression, game_player_id player_id, int32_t face) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_EMOTE)
		.add<game_player_id>(player_id)
		.add<int32_t>(face);
	return builder;
}

PACKET_IMPL(show_chat, game_player_id player_id, bool is_gm, const string &msg, bool bubble_only) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_CHAT)
		.add<game_player_id>(player_id)
		.add<bool>(is_gm)
		.add<string>(msg)
		.add<bool>(bubble_only);
	return builder;
}

SPLIT_PACKET_IMPL(damage_player, game_player_id player_id, game_damage dmg, game_mob_id mob, uint8_t hit, int8_t type, uint8_t stance, game_skill_id no_damage_skill, const return_damage_data &pgmr) {
	split_packet_builder builder;
	const int8_t bump_damage = -1;
	const int8_t map_damage = -2;

	builder.map
		.add<packet_header>(SMSG_PLAYER_DAMAGE)
		.add<game_player_id>(player_id)
		.add<int8_t>(type);

	switch (type) {
		case map_damage:
			builder.map
				.add<game_damage>(dmg)
				.add<game_damage>(dmg);
			break;
		default:
			builder.map
				.add<game_damage>(pgmr.reduction > 0 ? pgmr.damage : dmg)
				.add<game_mob_id>(mob)
				.add<uint8_t>(hit)
				.add<uint8_t>(pgmr.reduction);

			if (pgmr.reduction > 0) {
				builder.map
					.add<bool>(pgmr.is_physical) // Maybe? No Mana Reflection on global to test with
					.add<game_map_object>(pgmr.map_mob_id)
					.unk<int8_t>(6)
					.add<point>(pgmr.pos);
			}

			builder.map
				.add<uint8_t>(stance)
				.add<game_damage>(dmg);

			if (no_damage_skill > 0) {
				builder.map.add<game_skill_id>(no_damage_skill);
			}
			break;
	}
	return builder;
}

PACKET_IMPL(show_info, ref_ptr<vana::channel_server::player> get_info, bool is_self) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_INFO)
		.add<game_player_id>(get_info->get_id())
		.add<game_player_level>(get_info->get_stats()->get_level())
		.add<game_job_id>(get_info->get_stats()->get_job())
		.add<game_fame>(get_info->get_stats()->get_fame())
		.add<bool>(false) // Married
		.add<string>("-") // Guild
		.add<string>("") // Guild alliance
		.add<bool>(is_self);

	get_info->get_pets()->pet_info_packet(builder);
	get_info->get_mounts()->mount_info_packet(builder);
	get_info->get_inventory()->wishlist_info_packet(builder);
	get_info->get_monster_book()->info_packet(builder);
	return builder;
}

PACKET_IMPL(whisper_player, const string &whisperer_name, game_channel_id channel, const string &message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_COMMAND)
		.add<int8_t>(0x12)
		.add<string>(whisperer_name)
		.add<int16_t>(channel)
		.add<string>(message);
	return builder;
}

PACKET_IMPL(find_player, const string &name, opt_int32_t map, uint8_t is, bool is_channel) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_COMMAND);
	if (map.is_initialized()) {
		builder
			.add<int8_t>(0x09)
			.add<string>(name)
			.add<int8_t>(is_channel ? 0x03 : 0x01)
			.add<int32_t>(map.get())
			.unk<int32_t>()
			.unk<int32_t>();
	}
	else {
		builder
			.add<int8_t>(0x0A)
			.add<string>(name)
			.add<int8_t>(is);
	}
	return builder;
}

SPLIT_PACKET_IMPL(use_melee_attack, game_player_id player_id, game_skill_id mastery_skill_id, game_skill_level mastery_level, const attack_data &attack) {
	split_packet_builder builder;
	int8_t hit_byte = (attack.targets * 0x10) + attack.hits;
	game_skill_id skill_id = attack.skill_id;
	bool is_meso_explosion = attack.is_meso_explosion;
	if (is_meso_explosion) {
		hit_byte = (attack.targets * 0x10) + 0x0A;
	}

	builder.map
		.add<packet_header>(SMSG_ATTACK_MELEE)
		.add<game_player_id>(player_id)
		.add<int8_t>(hit_byte)
		.add<game_skill_level>(attack.skill_level);

	if (skill_id != constant::skill::all::regular_attack) {
		builder.map.add<game_skill_id>(skill_id);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weapon_speed)
		.add<uint8_t>(mastery_skill_id > 0 ? game_logic_utilities::get_mastery_display(mastery_level) : 0)
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<game_map_object>(target.first)
			.unk<int8_t>(0x06);

		if (is_meso_explosion) {
			builder.map.add<uint8_t>(static_cast<uint8_t>(target.second.size()));
		}
		for (const auto &hit : target.second) {
			builder.map.add<game_damage>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(use_ranged_attack, game_player_id player_id, game_skill_id mastery_skill_id, game_skill_level mastery_level, const attack_data &attack) {
	split_packet_builder builder;
	game_skill_id skill_id = attack.skill_id;

	builder.map
		.add<packet_header>(SMSG_ATTACK_RANGED)
		.add<game_player_id>(player_id)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<game_skill_level>(attack.skill_level);

	if (skill_id != constant::skill::all::regular_attack) {
		builder.map.add<game_skill_id>(skill_id);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weapon_speed)
		.add<uint8_t>(mastery_skill_id > 0 ? game_logic_utilities::get_mastery_display(mastery_level) : 0)
		// Bug in global:
		// The colored swoosh does not display as it should
		.add<game_item_id>(attack.star_id);

	for (const auto &target : attack.damages) {
		builder.map
			.add<game_map_object>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			game_damage damage = hit;
			switch (skill_id) {
				case constant::skill::marksman::snipe: // Snipe is always crit
					damage |= 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			builder.map.add<game_damage>(damage);
		}
	}
	builder.map.add<point>(attack.projectile_pos);
	return builder;
}

SPLIT_PACKET_IMPL(use_spell_attack, game_player_id player_id, const attack_data &attack) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_ATTACK_MAGIC)
		.add<game_player_id>(player_id)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<uint8_t>(attack.skill_level)
		.add<game_skill_id>(attack.skill_id)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weapon_speed)
		.add<uint8_t>(0) // Mastery byte is always 0 because spells don't have a swoosh
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<game_map_object>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<game_damage>(hit);
		}
	}

	if (attack.charge > 0) {
		builder.map.add<game_charge_time>(attack.charge);
	}
	return builder;
}

SPLIT_PACKET_IMPL(use_summon_attack, game_player_id player_id, const attack_data &attack) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_SUMMON_ATTACK)
		.add<game_player_id>(player_id)
		.add<game_summon_id>(attack.summon_id)
		.add<int8_t>(attack.animation)
		.add<int8_t>(attack.targets);

	for (const auto &target : attack.damages) {
		builder.map
			.add<game_map_object>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<game_damage>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(use_bomb_attack, game_player_id player_id, game_charge_time charge_time, game_skill_id skill_id, const wide_point &pos) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_3RD_PARTY_BOMB)
		.add<game_player_id>(player_id)
		.add<wide_point>(pos)
		.add<game_charge_time>(charge_time)
		.add<game_skill_id>(skill_id);
	return builder;
}

SPLIT_PACKET_IMPL(use_energy_charge_attack, game_player_id player_id, int32_t mastery_skill_id, uint8_t mastery_level, const attack_data &attack) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_ATTACK_ENERGYCHARGE)
		.add<game_player_id>(player_id)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<game_skill_level>(attack.skill_level)
		.add<game_skill_id>(attack.skill_id)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weapon_speed)
		.add<uint8_t>(mastery_skill_id > 0 ? game_logic_utilities::get_mastery_display(mastery_level) : 0)
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<game_map_object>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<game_damage>(hit);
		}
	}
	return builder;
}

}
}
}
}