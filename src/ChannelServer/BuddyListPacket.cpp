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
#include "BuddyListPacket.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerBuddyList.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace buddy {

PACKET_IMPL(error, uint8_t error) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_BUDDY)
		.add<int8_t>(error);
	return builder;
}

PACKET_IMPL(update, ref_ptr<player> player, uint8_t type) {
	packet_builder builder;
	uint8_t size = player->get_buddy_list()->list_size();

	builder
		.add<packet_header>(SMSG_BUDDY)
		.add<int8_t>(type)
		.add<uint8_t>(size);

	player->get_buddy_list()->add_buddies(builder);

	for (uint8_t i = 0; i < size; i++) {
		builder.unk<int32_t>();
	}
	return builder;
}

PACKET_IMPL(show_size, ref_ptr<player> player) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_BUDDY)
		.add<int8_t>(0x15)
		.add<uint8_t>(player->get_buddy_list_size());
	return builder;
}

PACKET_IMPL(invitation, const buddy_invite &invite) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_BUDDY)
		.add<int8_t>(0x09)
		.add<game_player_id>(invite.id)
		.add<string>(invite.name)
		.add<game_player_id>(invite.id)
		.add<string>(invite.name, 13)
		.add<uint8_t>(opposite_status::requested) // Buddy status
		.unk<int32_t>(-1) // Doesn't appear to matter
		.add<string>("Default Group", 13) // Needs to be set to "Default Group", because it's automatically added.
		.unk<int8_t>()
		.add<int8_t>(20) // Seems to be the amount of buddy slots for the character...
		.unk<uint8_t>(0xFD)
		.unk<uint8_t>(0xBA)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(online, game_player_id char_id, game_channel_id channel, bool cash_shop) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_BUDDY)
		.add<int8_t>(action_types::logon)
		.add<game_player_id>(char_id)
		.unk<int8_t>()
		.add<int32_t>(cash_shop ? 20 : channel);
	return builder;
}

}
}
}
}