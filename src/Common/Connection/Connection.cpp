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

#include "Connection.h"
#include <ListenSocket.h>
#include <iostream>

Connection * Connection::singleton = 0;

Connection::Connection() {
	terminate = false;
	selecting = false;
}

void Connection::accept(short port, AbstractPlayerFactory *apf) {
	ListenSocket<MapleSocket> *l = new ListenSocket<MapleSocket>(h);
	l->GetCreator()->SetAbstractPlayerFactory(apf);

	if (l->Bind(port)) {
		std::cout << "Failed to bind port " << port << std::endl;
		exit(2);
	}

	h.Add(l);

	startSelect();
}

AbstractPlayer * Connection::connect(const char *ip, short port, AbstractPlayerFactory *apf) {
	MapleSocket *s = new MapleSocket(h);
	s->SetAbstractPlayerFactory(apf);
	s->Open(ip, port);
	h.Add(s);

	startSelect();
	while (!s->GetReady()) {} // Wait till connection is ready
	return s->GetPlayer();
}

void Connection::startSelect() {
	if (!selecting) {
		HANDLE thread = CreateThread(NULL, 20000,
			(LPTHREAD_START_ROUTINE) &Connection::_select,
			(LPVOID) this,
			NULL,
			NULL);
	}
}

void Connection::select() {
	selecting = true;
	while (!terminate) {
		h.Select(0, 2000);
	}
	selecting = false;
}

void Connection::_select(Connection *obj) {
	obj->select();
}
