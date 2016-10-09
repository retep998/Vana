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
#include "effect_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {

PACKET_IMPL(play_music, const string &music) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x06)
		.add<string>(music);
	return builder;
}

PACKET_IMPL(send_event, const string &id) {
	packet_builder builder;
	// Look in Map.wz/Effect.img to find valid strings
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x03)
		.add<string>(id);
	return builder;
}

PACKET_IMPL(send_effect, const string &effect) {
	packet_builder builder;
	// Look in Map.wz/Obj/Effect.img/quest/ for valid strings
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x02)
		.add<string>(effect);
	return builder;
}

PACKET_IMPL(play_portal_sound_effect) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(0x07);
	return builder;
}

PACKET_IMPL(send_field_sound, const string &sound) {
	packet_builder builder;
	// Look in Sound.wz/Field.img to find valid strings
	builder
		.add<packet_header>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x04)
		.add<string>(sound);
	return builder;
}

PACKET_IMPL(send_minigame_sound, const string &sound) {
	packet_builder builder;
	// Look in Sound.wz/MiniGame.img to find valid strings
	builder
		.add<packet_header>(SMSG_SOUND)
		.add<string>(sound);
	return builder;
}

SPLIT_PACKET_IMPL(send_mob_item_buff_effect, game_player_id player_id, game_item_id item_id) {
	split_packet_builder builder;
	packet_builder buffer;
	buffer
		.add<int8_t>(0x0B)
		.add<game_item_id>(item_id);

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