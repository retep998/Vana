/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "ChannelServer.h"
#include "ConfigFile.h"
#include "ConnectionManager.h"
#include "InitializeChannel.h"
#include "InitializeCommon.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "ServerPacket.h"
#include "WorldServerConnection.h"

ChannelServer * ChannelServer::singleton = 0;

void ChannelServer::listen() {
	ConnectionManager::Instance()->accept(port, new PlayerFactory());
	Initializing::setUsersOffline(getOnlineId());
}

void ChannelServer::shutdown() {
	channel = -1; // Else when WorldServerConnection disconnects, it will try to call shutdown() again
	AbstractServer::shutdown();
}

void ChannelServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::checkMcdbVersion();
	Initializing::loadData();

	WorldServerConnection *loginPlayer = new WorldServerConnection;
	ConnectionManager::Instance()->connect(login_ip, login_inter_port, loginPlayer);
	loginPlayer->sendAuth(inter_password, external_ip);
}

void ChannelServer::connectWorld() {
	worldPlayer = new WorldServerConnection;
	ConnectionManager::Instance()->connect(world_ip, world_port, worldPlayer);
	worldPlayer->sendAuth(inter_password, external_ip);
}

void ChannelServer::loadConfig() {
	ConfigFile config("conf/channelserver.lua");
	login_ip = IpUtilities::stringToIp(config.getString("login_ip"));
	login_inter_port = config.getShort("login_inter_port");

	 // Will get from world server
	world = -1;
	port = -1;
	channel = -1;
	exprate = 1;
	questexprate = 1;
	mesorate = 1;
	droprate = 1;
	maxStats = 999;
	pianusChannel = false;
	papChannel = false;
	zakumChannel = false;
	horntailChannel = false;
	pinkbeanChannel = false;
}

void ChannelServer::sendToWorld(PacketCreator &packet) {
	worldPlayer->getSession()->send(packet);
}

void ChannelServer::setScrollingHeader(const string &message) {
	if (scrollingHeader != message) {
		scrollingHeader = message;
		if (scrollingHeader.size() == 0) {
			ServerPacket::scrollingHeaderOff();
		}
		else {
			ServerPacket::changeScrollingHeader(scrollingHeader);
		}
	}
}
