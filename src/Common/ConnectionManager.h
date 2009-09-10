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
#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "MapleServerClient.h"
#include "MapleServer.h"
#include "SessionManager.h"
#include "Types.h"
#include <list>
#include <boost/tr1/memory.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

using std::string;

class AbstractPlayerFactory;
class MapleServer;

class ConnectionManager : private boost::noncopyable {
public:
	static ConnectionManager * Instance() {
		if (singleton == 0)
			singleton = new ConnectionManager;
		return singleton;
	}

	void accept(uint16_t port, AbstractPlayerFactory *apf, string ivUnknown = "");
	void connect(uint32_t server, uint16_t port, AbstractPlayer *player);
	void stop();

	void run();
	void join();
private:
	void handle_run();
	void handle_stop();

	ConnectionManager();
	static ConnectionManager *singleton;

	boost::scoped_ptr<boost::thread> m_thread;
	boost::asio::io_service m_io_service;
	SessionManagerPtr m_clients;
	std::list<MapleServerPtr> m_servers;
	boost::scoped_ptr<boost::asio::io_service::work> m_work; // "Work" for io_service
};

#endif
