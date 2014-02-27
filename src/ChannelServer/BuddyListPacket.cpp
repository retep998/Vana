/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Player.hpp"
#include "PlayerBuddyList.hpp"
#include "SmsgHeader.hpp"

namespace BuddyListPacket {

PACKET_IMPL(error, uint8_t error) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_BUDDY)
		.add<int8_t>(error);
	return builder;
}

PACKET_IMPL(update, Player *player, uint8_t type) {
	PacketBuilder builder;
	uint8_t size = player->getBuddyList()->listSize();

	builder
		.add<header_t>(SMSG_BUDDY)
		.add<int8_t>(type)
		.add<uint8_t>(size);

	player->getBuddyList()->addBuddies(builder);

	for (uint8_t i = 0; i < size; i++) {
		builder.add<int32_t>(0);
	}
	return builder;
}

PACKET_IMPL(showSize, Player *player) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_BUDDY)
		.add<int8_t>(0x15)
		.add<uint8_t>(player->getBuddyListSize());
	return builder;
}

PACKET_IMPL(invitation, const BuddyInvite &invite) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_BUDDY)
		.add<int8_t>(0x09)
		.add<player_id_t>(invite.id)
		.add<string_t>(invite.name)
		.add<player_id_t>(invite.id)
		.add<string_t>(invite.name, 13)
		.add<uint8_t>(OppositeStatus::Requested) // Buddy status
		.add<int32_t>(-1) // Doesn't appear to matter
		.add<string_t>("Default Group", 13) // Needs to be set to "Default Group", because it's automatically added.
		.add<int8_t>(0x00)
		.add<int8_t>(20) // Seems to be the amount of buddy slots for the character...
		.add<uint8_t>(0xFD)
		.add<uint8_t>(0xBA)
		.add<int8_t>(0); // Unknown
	return builder;
}

PACKET_IMPL(online, player_id_t charId, channel_id_t channel) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_BUDDY)
		.add<int8_t>(0x14)
		.add<player_id_t>(charId)
		.add<int8_t>(0)
		.add<int32_t>(channel);
	return builder;
}

}