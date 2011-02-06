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
#include "WorldServerAcceptConnection.h"
#include "Channels.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "LoginServerConnectPacket.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "PacketReader.h"
#include "PlayerDataProvider.h"
#include "Rates.h"
#include "SyncHandler.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include "WorldServerAcceptHandler.h"
#include "WorldServerAcceptPacket.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

WorldServerAcceptConnection::~WorldServerAcceptConnection() {
	if (isAuthenticated()) {
		if (getType() == ServerTypes::Channel) {
			if (WorldServer::Instance()->isConnected()) {
				LoginServerConnectPacket::removeChannel(channel);
			}
			PlayerDataProvider::Instance()->removeChannelPlayers(channel);
			Channels::Instance()->removeChannel(channel);

			WorldServer::Instance()->log(LogTypes::ServerDisconnect, "Channel " + boost::lexical_cast<string>(channel));
		}
		else if (getType() == ServerTypes::Cash) {
			if (WorldServer::Instance()->isConnected()) {
				LoginServerConnectPacket::removeCashServer();
			}
			
			WorldServer::Instance()->setCashServerConnected(false);
			WorldServer::Instance()->setCashServer(nullptr);
			WorldServer::Instance()->log(LogTypes::ServerDisconnect, "Cash server");
		}
	}
}

void WorldServerAcceptConnection::realHandleRequest(PacketReader &packet) {
	if (!processAuth(WorldServer::Instance(), packet, WorldServer::Instance()->getInterPassword())) return;
	switch (packet.getHeader()) {
		case IMSG_SYNC: SyncHandler::handle(this, packet); break;
		case IMSG_FIND: WorldServerAcceptHandler::findPlayer(this, packet); break;
		case IMSG_WHISPER: WorldServerAcceptHandler::whisperPlayer(this, packet); break;
		case IMSG_SCROLLING_HEADER: WorldServerAcceptHandler::scrollingHeader(this, packet); break;
		case IMSG_GROUP_CHAT: WorldServerAcceptHandler::groupChat(this, packet); break;
		case IMSG_TO_LOGIN: WorldServerAcceptHandler::sendToLogin(packet); break;
		case IMSG_TO_CHANNELS: WorldServerAcceptHandler::sendToChannels(packet); break;
		case IMSG_TO_PLAYERS: packet.reset(); WorldServerAcceptHandler::sendToChannels(packet); break;
	}
}

void WorldServerAcceptConnection::authenticated(int8_t type) {
	if (type == ServerTypes::Channel) {
		channel = Channels::Instance()->getAvailableChannel();
		if (channel != -1) {
			uint16_t port = WorldServer::Instance()->getInterPort() + channel + 1;
			Channels::Instance()->registerChannel(this, channel, getIp(), getExternalIp(), port);
			WorldServerAcceptPacket::connect(this, channel, port);
			WorldServerAcceptPacket::sendRates(this, Rates::SetBits::all);
			WorldServerAcceptPacket::scrollingHeader(WorldServer::Instance()->getScrollingHeader());
			SyncPacket::PlayerPacket::sendParties(this);
			SyncPacket::PlayerPacket::sendGuilds(this);
			SyncPacket::PlayerPacket::sendAlliances(this);
			LoginServerConnectPacket::registerChannel(channel, getIp(), getExternalIp(), port);

			WorldServer::Instance()->log(LogTypes::ServerConnect, "Channel " + boost::lexical_cast<string>(channel));
		}
		else {
			WorldServerAcceptPacket::connect(this, -1, 0);
			WorldServer::Instance()->log(LogTypes::Info, "No more channels to assign.");
			getSession()->disconnect();
		}
	}
	else if (type == ServerTypes::Cash) {
		if (!WorldServer::Instance()->isCashServerConnected()) {
			uint16_t port = WorldServer::Instance()->getCashPort();
			WorldServer::Instance()->setCashServer(this);
			WorldServer::Instance()->setCashServerConnected(true);

			WorldServerAcceptPacket::connectCashServer(this, port, true);
			LoginServerConnectPacket::registerCashServer();
			WorldServer::Instance()->log(LogTypes::ServerConnect, "Cash server");
		}
		else {
			WorldServerAcceptPacket::connectCashServer(this, -1, false);
			WorldServer::Instance()->log(LogTypes::Info, "Cash server already assigned.");
			getSession()->disconnect();
		}
	}
	else {
		WorldServer::Instance()->log(LogTypes::Warning, "Unknown server tried to connect. IP: " + IpUtilities::ipToString(getIp()));
		getSession()->disconnect();
	}
}
