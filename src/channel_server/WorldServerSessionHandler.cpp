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
#include "WorldServerSessionHandler.hpp"
#include "common_temp/BeautyDataProvider.hpp"
#include "common_temp/DropDataProvider.hpp"
#include "common_temp/ExitCodes.hpp"
#include "common_temp/ItemDataProvider.hpp"
#include "common_temp/MobDataProvider.hpp"
#include "common_temp/PacketReader.hpp"
#include "common_temp/PacketWrapper.hpp"
#include "common_temp/QuestDataProvider.hpp"
#include "common_temp/ReactorDataProvider.hpp"
#include "common_temp/ScriptDataProvider.hpp"
#include "common_temp/Session.hpp"
#include "common_temp/ShopDataProvider.hpp"
#include "common_temp/SkillDataProvider.hpp"
#include "common_temp/WorldConfig.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/MapDataProvider.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerPacket.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayersPacket.hpp"
#include "channel_server/WorldServerSession.hpp"
#include <iostream>
#include <limits>

namespace vana {
namespace channel_server {

auto world_server_session_handler::connect(ref_ptr<world_server_session> session, packet_reader &reader) -> void {
	game_channel_id channel = reader.get<game_channel_id>();
	if (channel != -1) {
		connection_port port = reader.get<connection_port>();
		world_config conf = reader.get<world_config>();
		channel_server::get_instance().established_world_connection(channel, port, conf);
	}
	else {
		channel_server::get_instance().log(log_type::critical_error, "no channel to handle");
		channel_server::get_instance().shutdown();
	}
}

auto world_server_session_handler::reload_mcdb(packet_reader &reader) -> void {
	string args = reader.get<string>();
	channel_server::get_instance().reload_data(args);
}

}
}