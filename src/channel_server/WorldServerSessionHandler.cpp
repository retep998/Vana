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
#include "common/BeautyDataProvider.hpp"
#include "common/DropDataProvider.hpp"
#include "common/ExitCodes.hpp"
#include "common/ItemDataProvider.hpp"
#include "common/MobDataProvider.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWrapper.hpp"
#include "common/QuestDataProvider.hpp"
#include "common/ReactorDataProvider.hpp"
#include "common/ScriptDataProvider.hpp"
#include "common/Session.hpp"
#include "common/ShopDataProvider.hpp"
#include "common/SkillDataProvider.hpp"
#include "common/WorldConfig.hpp"
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