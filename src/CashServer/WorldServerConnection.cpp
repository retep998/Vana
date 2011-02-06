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
#include "WorldServerConnection.h"
#include "CashDataProvider.h"
#include "CashServer.h"
#include "InterHeader.h"
#include "InitializeCashServer.h"
#include "PacketReader.h"
#include "SyncHandler.h"
#include "WorldServerConnectHandler.h"
#include "WorldServerConnectPacket.h"
#include <iostream>

WorldServerConnection::WorldServerConnection() {
	setType(ServerTypes::Cash);
}

WorldServerConnection::~WorldServerConnection() {
	if (CashServer::Instance()->isConnected()) {
		std::cout << "Disconnected from the worldserver. Shutting down..." << std::endl;
		CashServer::Instance()->shutdown();
	}
}

void WorldServerConnection::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: WorldServerConnectHandler::connectLogin(this, packet); break;
		case IMSG_CASH_SERVER_CONNECT: WorldServerConnectHandler::connect(this, packet); break;
		case IMSG_SCROLLING_HEADER: WorldServerConnectHandler::scrollingHeader(packet); break;
		case IMSG_REFRESH_DATA: Initializing::loadData(); break;
		case IMSG_UPDATE_COUPON: WorldServerConnectHandler::updateCoupon(packet); break;
		case IMSG_RECALCULATE_BEST_ITEMS: CashDataProvider::Instance()->loadBestItems(); break;

		case IMSG_SYNC: SyncHandler::handle(packet); break;
	}
}
