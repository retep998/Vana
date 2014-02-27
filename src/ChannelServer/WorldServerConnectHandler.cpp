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
#include "WorldServerConnectHandler.hpp"
#include "BeautyDataProvider.hpp"
#include "ChannelServer.hpp"
#include "Configuration.hpp"
#include "DropDataProvider.hpp"
#include "ItemDataProvider.hpp"
#include "MapDataProvider.hpp"
#include "MobDataProvider.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayersPacket.hpp"
#include "QuestDataProvider.hpp"
#include "ReactorDataProvider.hpp"
#include "ScriptDataProvider.hpp"
#include "Session.hpp"
#include "ShopDataProvider.hpp"
#include "SkillDataProvider.hpp"
#include "WorldServerConnection.hpp"
#include <iostream>
#include <limits>

auto WorldServerConnectHandler::connectLogin(WorldServerConnection *player, PacketReader &reader) -> void {
	world_id_t worldId = reader.get<world_id_t>();
	if (worldId != -1) {
		Ip ip = reader.get<Ip>();
		port_t port = reader.get<port_t>();
		std::cout << "Connecting to world " << static_cast<int32_t>(worldId) << std::endl;
		ChannelServer::getInstance().connectToWorld(worldId, port, ip);
	}
	else {
		std::cerr << "ERROR: No world server to connect" << std::endl;
		ChannelServer::getInstance().shutdown();
	}
}

auto WorldServerConnectHandler::connect(WorldServerConnection *player, PacketReader &reader) -> void {
	channel_id_t channel = reader.get<channel_id_t>();
	if (channel != -1) {
		port_t port = reader.get<port_t>();
		WorldConfig conf = reader.get<WorldConfig>();
		std::cout << "Handling channel " <<static_cast<int32_t>(channel) << " on port " << port << std::endl;
		ChannelServer::getInstance().establishedWorldConnection(channel, port, conf);
	}
	else {
		std::cerr << "ERROR: No channel to handle" << std::endl;
		ChannelServer::getInstance().shutdown();
	}
}

auto WorldServerConnectHandler::reloadMcdb(PacketReader &reader) -> void {
	string_t args = reader.get<string_t>();
	if (args == "all") {
		ItemDataProvider::getInstance().loadData();
		DropDataProvider::getInstance().loadData();
		ShopDataProvider::getInstance().loadData();
		MobDataProvider::getInstance().loadData();
		BeautyDataProvider::getInstance().loadData();
		ScriptDataProvider::getInstance().loadData();
		SkillDataProvider::getInstance().loadData();
		ReactorDataProvider::getInstance().loadData();
		QuestDataProvider::getInstance().loadData();
	}
	else if (args == "items") ItemDataProvider::getInstance().loadData();
	else if (args == "drops") DropDataProvider::getInstance().loadData();
	else if (args == "shops") ShopDataProvider::getInstance().loadData();
	else if (args == "mobs") MobDataProvider::getInstance().loadData();
	else if (args == "beauty") BeautyDataProvider::getInstance().loadData();
	else if (args == "scripts") ScriptDataProvider::getInstance().loadData();
	else if (args == "skills") SkillDataProvider::getInstance().loadData();
	else if (args == "reactors") ReactorDataProvider::getInstance().loadData();
	else if (args == "quest") QuestDataProvider::getInstance().loadData();
}