/*
Copyright (C) 2008 Vana Development Team

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
#include "WorldServer.h"
#include "WorldServerAcceptPlayerPacket.h"
#include "Acceptor.h"
#include "Connector.h"

WorldServer * WorldServer::singleton = 0;

void WorldServer::listen() {
	new Acceptor(inter_port, new WorldServerAcceptPlayerFactory());
}

void WorldServer::loadData() {
	Connector *c = new Connector(login_ip, login_inter_port, new LoginServerConnectPlayerFactory());
	loginPlayer = (LoginServerConnectPlayer *) c->getPlayer();
	loginPlayer->setIP(external_ip);
	loginPlayer->sendAuth(inter_password);
}

void WorldServer::loadConfig() {
	Config config("conf/worldserver.lua");
	login_ip = config.getString("login_ip");
	login_inter_port = config.getInt("login_inter_port");
	external_ip = config.getString("external_ip"); // External IP
	inter_port = -1; // Will get from login server later
}

void WorldServer::shutdown() {
	exit(0);
	//TODO
}

void WorldServer::setScrollingHeader(const string &message) {
	scrollingHeader = message;
	WorldServerAcceptPlayerPacket::scrollingHeader(message);
}
