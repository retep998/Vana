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
#include "AbstractPlayer.h"
#include "ConnectionManager.h"
#include <memory>

ConnectionManager * ConnectionManager::singleton = 0;

void ConnectionManager::accept(uint16_t port, AbstractPlayerFactory *apf, string ivUnknown) {
	tcp::endpoint endpoint(tcp::v4(), port);
	m_servers.push_back(MapleServerPtr(new MapleServer(m_io_service, endpoint, apf, ivUnknown)));

	run();
}

AbstractPlayer * ConnectionManager::connect(const string &server, uint16_t port,
											AbstractPlayerFactory *apf) {
	AbstractPlayer *player =  apf->createPlayer();
	MapleClientPtr c = MapleClientPtr(new MapleClient(m_io_service, server, port, m_clients, player));
	c->start_connect();

	run();

	return player;
}

void ConnectionManager::run() {
	if (!m_running) {
		m_thread.reset(new boost::thread(std::tr1::bind(&ConnectionManager::runThread, this)));
		m_running = true;
	}
}

// Don't call this directly, call run()
void ConnectionManager::runThread() {
	m_io_service.run();
}
