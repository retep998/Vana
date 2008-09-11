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
#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "MapleClient.h"
#include "MapleServer.h"
#include "SessionManager.h"
#include <list>
#include <memory>
#include <string>
#include <boost/noncopyable.hpp>
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

	void accept(unsigned short port, AbstractPlayerFactory *apf, string ivUnknown = "");
	AbstractPlayer * connect(const string &server, unsigned short port,
		AbstractPlayerFactory *apf);
private:
	void run();
	void runThread();

	ConnectionManager() : m_running(false), m_clients(new SessionManager) { }
	static ConnectionManager *singleton;

	volatile bool m_running;
	std::tr1::shared_ptr<boost::thread> m_thread;
	boost::asio::io_service m_io_service;
	SessionManagerPtr m_clients;
	std::list<MapleServerPtr> m_servers;
};

#endif