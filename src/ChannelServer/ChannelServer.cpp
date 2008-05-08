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
#include "ChannelServer.h"

ChannelServer* ChannelServer::singleton = 0;

void ChannelServer::listen() {
	selector = new Selector();
	new Acceptor(_port, selector, new PlayerFactory());
}

void ChannelServer::loadData() {
	Initializing::initializeMySQL();
	Initializing::initializeMobs();
	Initializing::initializeItems();
	Initializing::initializeDrops();
	Initializing::initializeEquips();
	Initializing::initializeShops();
	Initializing::initializeQuests();
	Initializing::initializeSkills();
	Initializing::initializeMaps();
	Initializing::initializeTimers();
}

void ChannelServer::loadConfig() {
	_port = 8888; //TODO: Get port from world server
}

void ChannelServer::shutdown() {
	// Note that we are not increasing an iterator because deleting a player would cause the iterator to fail
	while (1) {
		if (Players::players.empty())
			break;
		hash_map<int,Player*>::iterator iter = Players::players.begin();
		iter->second->disconnect();
	}
	exit(0);
}
