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
#include "login_packet.hpp"
#include "common/client_ip.hpp"
#include "login_server/channel.hpp"
#include "login_server/characters.hpp"
#include "login_server/login_packet_helper.hpp"
#include "login_server/player_status.hpp"
#include "login_server/smsg_header.hpp"
#include "login_server/user.hpp"
#include "login_server/world.hpp"

namespace vana {
namespace login_server {
namespace packets {

PACKET_IMPL(login_error, int16_t error_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_AUTHENTICATION)
		.add<int16_t>(error_id)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(login_ban, int8_t reason, file_time expire) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_AUTHENTICATION)
		.add<int16_t>(2)
		.unk<int32_t>()
		.add<int8_t>(reason)
		.add<file_time>(expire);
	// Expiration will be a permanent ban if it's more than 2 years over the current year
	// However, the cutoff is Jan1
	// So if it's June, 2008 on the client's system, sending Jan 1 2011 = permanent ban
	return builder;
}

PACKET_IMPL(login_connect, ref_ptr<user> user_value, const string &username) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_AUTHENTICATION)
		.add<int16_t>(0)
		.unk<int32_t>()
		.add<game_account_id>(user_value->get_account_id());

	switch (user_value->get_status()) {
		case player_status::set_gender:
			builder.add<int8_t>(player_status::set_gender);
			break;
		case player_status::set_pin:
			builder.add<int8_t>(player_status::pin_select);
			break;
		default:
			builder.add<game_gender_id>(user_value->get_gender().get(-1));
			break;
	}

	// GradeCode
	// & 0x20 == gmlevel 0
	// & 0x10
	// & 0x1 || (??2087) == gmlevel 1
	//  Notes
	//  - No warning about items that will disappear when you drop them (also for 0x10)
	//  - Disables namechecks
	//  - Ignores hideMinimap prop of maps
	//  - Does not disable 
	builder.add<bool>(user_value->is_admin()); // Enables commands like /c, /ch, /m, /h... but disables trading

	// Seems like 0x80 is a "MWLB" account - I doubt it... it disables attacking and allows GM fly
	// 0x40, 0x20 (and probably 0x10, 0x8, 0x4, 0x2, and 0x1) don't appear to confer any particular benefits, restrictions, or functionality
	// (Although I didn't test client GM commands or anything of the sort)

	// SubGradeCode
	// & 0x80 == usergm == gmlevel 5 (note 1 << 7)
	//  Notes:
	//  - Enables GM fly
	//  - Will send /u command on enter field
	//  - Disables attacking (because you will be a MWLB)
	// & 0x40 = gmlevel 3 (subgm)
	// & 0x20 = gmlevel 2 (subgm)
	builder.add<uint8_t>(user_value->is_admin() ? 0x80 : 0x00);


	builder.add<uint8_t>(0); // Country code

	builder
		.add<string>(username)
		.unk<int8_t>()
		.add<int8_t>(user_value->get_quiet_ban_reason())
		.add<file_time>(user_value->get_quiet_ban_time())
		.add<file_time>(user_value->get_creation_time())
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(login_process, int8_t id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PIN)
		.add<int8_t>(id);
	return builder;
}

PACKET_IMPL(pin_assigned) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PIN_ASSIGNED)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(gender_done, game_gender_id gender) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_ACCOUNT_GENDER_DONE)
		.add<game_gender_id>(gender)
		.unk<int8_t>(1);
	return builder;
}

PACKET_IMPL(show_world, world *world_value) {
	packet_builder builder;

	// Modifying this will show the event message
	int16_t exp_rate_percentage = 100;

	builder
		.add<packet_header>(SMSG_WORLD_LIST)
		.add<game_world_id>(world_value->get_id().get(-1))
		.add<string>(world_value->get_name())
		.add<int8_t>(world_value->get_ribbon())
		.add<string>(world_value->get_event_message())
		.add<int16_t>(exp_rate_percentage) 
		.unk<int16_t>(100)
		.unk<int8_t>();

	builder.add<game_channel_id>(world_value->get_max_channels());
	for (game_channel_id i = 0; i < world_value->get_max_channels(); i++) {
		out_stream cn_stream;
		cn_stream << world_value->get_name() << "-" << static_cast<int32_t>(i + 1);
		builder.add<string>(cn_stream.str());

		if (channel *chan = world_value->get_channel(i)) {
			builder.add<int32_t>(chan->get_population());
		}
		else {
			// Channel doesn't exist
			builder.add<int32_t>(0);
		}

		builder
			.add<game_world_id>(world_value->get_id().get(-1))
			.add<uint8_t>(i)
			.unk<uint8_t>(); // Some sort of state
	}

	int16_t message_count = 0;
	builder.add<int16_t>(message_count);
	for (int16_t i = 0; i < message_count; ++i) {
		// When you set a pos of (0, 0), the message will be on the first world tab
		builder.add<point>(point{i * 10, 0});
		builder.add<string>("message");
	}

	return builder;
}

PACKET_IMPL(world_end) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_WORLD_LIST)
		.add<game_world_id>(-1);
	return builder;
}

PACKET_IMPL(show_channels, int8_t status) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_WORLD_STATUS)
		.add<int16_t>(status);
	return builder;
}

PACKET_IMPL(channel_select) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANNEL_SELECT)
		.unk<int16_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(show_characters, const vector<character> &chars, int32_t max_chars) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_LIST)
		.unk<int8_t>();

	builder.add<uint8_t>(static_cast<uint8_t>(chars.size()));
	for (const auto &elem : chars) {
		builder.add_buffer(helpers::add_character(elem));
	}

	builder.add<int32_t>(max_chars);
	return builder;
}

PACKET_IMPL(channel_offline) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_LIST)
		.add<int8_t>(8);
	return builder;
}

PACKET_IMPL(check_name, const string &name, uint8_t message) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_NAME_CHECK)
		.add<string>(name)
		.add<uint8_t>(message);
	return builder;
}

PACKET_IMPL(show_all_characters_info, game_world_id world_count, uint32_t unk) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_GLOBAL_LIST)
		.add<bool>(true) // Indicates the world-specific character list, true means it's the world-specific character list
		.add<int32_t>(world_count)
		.unk<uint32_t>(unk);
	return builder;
}

PACKET_IMPL(show_view_all_characters, game_world_id world_id, const vector<character> &chars) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_GLOBAL_LIST)
		.add<bool>(false) // Indicates the world-specific character list, false means it's view all characters
		.add<game_world_id>(world_id);

	builder.add<uint8_t>(static_cast<uint8_t>(chars.size()));
	for (const auto &elem : chars) {
		builder.add_buffer(helpers::add_character(elem));
	}
	return builder;
}

PACKET_IMPL(show_character, const character &charc) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_CREATE)
		.unk<int8_t>()
		.add_buffer(helpers::add_character(charc));
	return builder;
}

PACKET_IMPL(delete_character, game_player_id id, uint8_t result) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_DELETE)
		.add<game_player_id>(id)
		.add<uint8_t>(result);
	return builder;
}

PACKET_IMPL(connect_ip, const optional<client_ip> &ip_value, optional<connection_port> port, game_player_id char_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHANNEL_CONNECT)
		.unk<int16_t>();

	if (ip_value.is_initialized() && port.is_initialized()) {
		builder
			.add<client_ip>(ip_value.get())
			.add<connection_port>(port.get());
	}
	else {
		builder
			.add<client_ip>(client_ip{ip{0}})
			.add<connection_port>(-1);
	}

	builder
		.add<game_player_id>(char_id)
		.unk<int32_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(relog_response) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_LOGIN_RETURN)
		.unk<int8_t>(1);
	return builder;
}

}
}
}