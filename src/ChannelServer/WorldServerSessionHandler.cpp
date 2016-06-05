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
#include "Common/BeautyDataProvider.hpp"
#include "Common/DropDataProvider.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/MobDataProvider.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/QuestDataProvider.hpp"
#include "Common/ReactorDataProvider.hpp"
#include "Common/ScriptDataProvider.hpp"
#include "Common/Session.hpp"
#include "Common/ShopDataProvider.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/WorldConfig.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/MapDataProvider.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerPacket.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayersPacket.hpp"
#include "ChannelServer/WorldServerSession.hpp"
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