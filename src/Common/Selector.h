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
#ifndef SELECTOR_H
#define SELECTOR_H

#include <Winsock2.h>
#include <hash_map>

#define BOOST_ALL_DYN_LINK
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

using stdext::hash_map;
using boost::scoped_ptr;

class Selector {
public:
	static Selector * Instance() {
		if (singleton == 0)
			singleton = new Selector;
		return singleton;
	}

	class Handler {
	public:
		Handler() : destroy (false) { }
		virtual ~Handler() { }
		virtual void handle (int socket) = 0;
		void setDestroy() { destroy = true; }
		bool getDestroy() const { return destroy; }
	private:
		bool destroy;
	};

	~Selector();

	void registerSocket(int socket, bool selectRead, bool selectWrite, bool selectError, Handler *handler);

	void unregisterSocket(int socket);
	void selectThread();

private:
	Selector();
	Selector(const Selector&);
	Selector& operator=(const Selector&);
	static Selector *singleton;

	bool terminate;
	fd_set readfds;
	fd_set writefds;
	fd_set errorfds;
	struct timeval timeout;
	hash_map<int, Handler *> handlers;
	scoped_ptr<boost::thread> selectorthread;
};

#endif
