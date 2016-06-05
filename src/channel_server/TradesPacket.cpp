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
#include "TradesPacket.hpp"
#include "common/session_temp.hpp"
#include "channel_server/Inventory.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerPacketHelper.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace trades {

PACKET_IMPL(send_open_trade, ref_ptr<player> player1, ref_ptr<player> player2) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x05)
		.add<int8_t>(0x03)
		.add<int8_t>(0x02)
		.add<int16_t>((player1 != nullptr && player2 != nullptr) ? 1 : 0);

	if (player2 != nullptr) {
		builder
			.add_buffer(helpers::add_player_display(player2))
			.add<string>(player2->get_name())
			.add<int8_t>(1); // Location in the window
	}
	if (player1 != nullptr) {
		builder
			.add_buffer(helpers::add_player_display(player1))
			.add<string>(player1->get_name())
			.add<int8_t>(-1); // Location in the window
	}
	return builder;
}

PACKET_IMPL(send_trade_request, const string &name, game_trade_id trade_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x02)
		.add<int8_t>(0x03)
		.add<string>(name)
		.add<game_trade_id>(trade_id);
	return builder;
}

PACKET_IMPL(send_trade_message, const string &name, int8_t type, int8_t message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(type)
		.add<int8_t>(message)
		.add<string>(name);
	return builder;
}

PACKET_IMPL(send_end_trade, int8_t message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(packets::trades::message_types::end_trade)
		.add<int8_t>(0x00)
		.add<int8_t>(message);
	return builder;
}

PACKET_IMPL(send_trade_entry_message, int8_t message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(packets::trades::message_types::shop_entry_messages)
		.add<int8_t>(0x00)
		.add<int8_t>(message);
	return builder;
}

PACKET_IMPL(send_trade_chat, bool blue, const string &chat) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x06)
		.add<int8_t>(0x08)
		.add<bool>(blue)
		.add<string>(chat);
	return builder;
}

PACKET_IMPL(send_add_user, ref_ptr<player> new_player, int8_t slot) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x04)
		.add<int8_t>(slot)
		.add_buffer(helpers::add_player_display(new_player))
		.add<string>(new_player->get_name());
	return builder;
}

PACKET_IMPL(send_leave_trade) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0A)
		.add<int8_t>(0x01) // Slot, doesn't matter for trades
		.add<int8_t>(0x02); // Message, doesn't matter for trades
	return builder;
}

PACKET_IMPL(send_add_mesos, uint8_t slot, game_mesos amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0F)
		.add<int8_t>(slot)
		.add<game_mesos>(amount);
	return builder;
}

PACKET_IMPL(send_accepted) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x10);
	return builder;
}

PACKET_IMPL(send_add_item, uint8_t player, uint8_t slot, item *item) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0E)
		.add<int8_t>(player)
		.add_buffer(helpers::add_item_info(slot, item));
	return builder;
}

}
}
}
}