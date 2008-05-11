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
#include "LoginServer.h"

LoginServer* LoginServer::singleton = 0;

void LoginServer::listen() {
	new Acceptor(port, new PlayerLoginFactory());
	new Acceptor(inter_port, new LoginServerAcceptPlayerFactory());
}

void LoginServer::loadData() {
	Initializing::initializeMySQL();
	Initializing::setUsersOffline(1);
}

void LoginServer::loadConfig() {
	Config config("conf/loginserver.lua");
	pinEnabled = config.getBool("pin");
	port = config.getInt("port");
	inter_port = config.getInt("inter_port");
}

void LoginServer::shutdown() {
	//TODO
}

