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

namespace Vana {
namespace ChannelServer {

auto WorldServerSessionHandler::connect(ref_ptr_t<WorldServerSession> session, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	if (channel != -1) {
		port_t port = reader.get<port_t>();
		WorldConfig conf = reader.get<WorldConfig>();
		ChannelServer::getInstance().establishedWorldConnection(channel, port, conf);
	}
	else {
		ChannelServer::getInstance().log(LogType::CriticalError, "No channel to handle");
		ChannelServer::getInstance().shutdown();
	}
}

auto WorldServerSessionHandler::reloadMcdb(PacketReader &reader) -> void {
	string_t args = reader.get<string_t>();
	ChannelServer::getInstance().reloadData(args);
}

}
}