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

LoginServer::LoginServer() {
	srand((unsigned char)time(0));
	loadConfig();
}

void LoginServer::initialize() {
	Initializing::initializeMySQL();

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// if (iResult != NO_ERROR)  printf("Error at WSAStartup()\n"); //TODO: Throw exception

	listen();
}

void LoginServer::listen() {
	selector = new Selector();
	Acceptor::Acceptor(_port, selector, new PlayerLoginFactory());
}

void LoginServer::loadConfig() {
	Config config("conf/login.lua");
	_pinEnabled = config.getBool("pin");
	_port = config.getInt("port");
}
