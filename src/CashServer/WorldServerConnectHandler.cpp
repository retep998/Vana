/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "WorldServerConnectHandler.h"
#include "CashServer.h"
#include "CashDataProvider.h"
#include "Configuration.h"
#include "ConfigurationPacket.h"
#include "ItemDataProvider.h"
#include "InitializeCashServer.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "QuestDataProvider.h"
#include "Quests.h"
#include "Rates.h"
#include "SkillDataProvider.h"
#include "WorldServerConnection.h"
#include <iostream>
#include <limits>

void WorldServerConnectHandler::connectLogin(WorldServerConnection *player, PacketReader &packet) {
	int8_t worldid = packet.get<int8_t>();
	int8_t type = packet.get<int8_t>();
	if (worldid != -1 && type == InterCashServer) {
		CashServer::Instance()->setWorld(worldid);
		CashServer::Instance()->setWorldIp(packet.get<uint32_t>());
		CashServer::Instance()->setWorldPort(packet.get<uint16_t>());
		std::cout << "Connecting to world " << (int16_t) worldid << std::endl;
		CashServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		CashServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnection *player, PacketReader &packet) {
	bool canHandle = packet.getBool();
	if (canHandle) {
		uint16_t port = packet.get<uint16_t>();
		CashServer::Instance()->setPort(port);
		CashServer::Instance()->setConnected(true);

		CashServer::Instance()->listen();
		std::cout << "Handling cash server for world " << (int32_t)CashServer::Instance()->getWorld() << " on port " << port << std::endl;

		CashServer::Instance()->displayLaunchTime();
	}
	else {
		std::cout << "Error: No cash server to handle" << std::endl;
		CashServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::scrollingHeader(PacketReader &packet) {
	string message = packet.getString();
	CashServer::Instance()->setScrollingHeader(message);
}

void WorldServerConnectHandler::updateCoupon(PacketReader &packet) {
	CouponInfo *info = CashDataProvider::Instance()->getCouponInfo(packet.getString());
	if (info != nullptr) {
		info->used = packet.getBool();
	}
}
