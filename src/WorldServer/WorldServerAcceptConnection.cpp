/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "LoginServerConnectPacket.h"
#include "MiscUtilities.h"
#include "PacketReader.h"
#include "PlayerDataProvider.h"
#include "Session.h"
#include "StringUtilities.h"
#include "SyncHandler.h"
#include "SyncPacket.h"
#include "VanaConstants.h"
#include "WorldServer.h"
#include "WorldServerAcceptHandler.h"
#include "WorldServerAcceptPacket.h"
#include <iostream>

WorldServerAcceptConnection::~WorldServerAcceptConnection() {
	if (isAuthenticated()) {
		if (getType() == ServerTypes::Channel) {
			if (WorldServer::Instance()->isConnected()) {
				LoginServerConnectPacket::removeChannel(m_channel);
			}
			PlayerDataProvider::Instance()->removeChannelPlayers(m_channel);
			Channels::Instance()->removeChannel(m_channel);

			WorldServer::Instance()->log(LogTypes::ServerDisconnect, "Channel " + StringUtilities::lexical_cast<string>(m_channel));
		}
	}
}

void WorldServerAcceptConnection::handleRequest(PacketReader &packet) {
	if (!processAuth(WorldServer::Instance(), packet)) return;
	switch (packet.getHeader()) {
		case IMSG_SYNC: SyncHandler::handle(this, packet); break;
		case IMSG_FIND: WorldServerAcceptHandler::findPlayer(this, packet); break;
		case IMSG_WHISPER: WorldServerAcceptHandler::whisperPlayer(this, packet); break;
		case IMSG_GROUP_CHAT: WorldServerAcceptHandler::groupChat(this, packet); break;
		case IMSG_TO_LOGIN: WorldServerAcceptHandler::sendToLogin(packet); break;
		case IMSG_TO_CHANNELS: WorldServerAcceptHandler::sendToChannels(packet); break;
		case IMSG_TO_PLAYERS: packet.reset(); WorldServerAcceptHandler::sendToChannels(packet); break;
	}
}

void WorldServerAcceptConnection::authenticated(int8_t type) {
	if (type == ServerTypes::Channel) {
		m_channel = Channels::Instance()->getAvailableChannel();
		if (m_channel != -1) {
			port_t port = WorldServer::Instance()->getInterPort() + m_channel + 1;
			Channels::Instance()->registerChannel(this, m_channel, getIp(), getExternalIp(), port);

			WorldServerAcceptPacket::connect(this, m_channel, port);
			SyncPacket::sendSyncData(this);
			LoginServerConnectPacket::registerChannel(m_channel, getIp(), getExternalIp(), port);

			WorldServer::Instance()->log(LogTypes::ServerConnect, "Channel " + StringUtilities::lexical_cast<string>(m_channel));
		}
		else {
			WorldServerAcceptPacket::connect(this, -1, 0);
			std::cerr << "Error: No more channels to assign." << std::endl;
			getSession()->disconnect();
		}
	}
}