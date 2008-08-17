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
#include "Selector.h"
#include <functional>

using std::tr1::bind;

Selector * Selector::singleton = 0;

Selector::Selector() : terminate(false) {
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&errorfds);

	selectorthread.reset(new boost::thread(bind(&Selector::selectThread, this)));
}

Selector::~Selector() {
	terminate = true;
}

void Selector::registerSocket(int socket, bool selectRead, bool selectWrite, bool selectError, Handler *handler) {
	if (selectRead) {
		FD_SET(socket, &readfds);
	}
	if (selectWrite) {
		FD_SET(socket, &writefds);
	}
	if (selectError) {
		FD_SET(socket, &errorfds);
	}
	handlers[socket] = handler;
}

void Selector::unregisterSocket(int socket) {
	FD_CLR(socket, &readfds);
	FD_CLR(socket, &writefds);
	FD_CLR(socket, &errorfds);
	handlers.erase(socket);
}

void Selector::selectThread() {
	fd_set t_readfds;
	fd_set t_writefds;
	fd_set t_errorfds;
	while (!terminate) {
		t_readfds = readfds;
		t_writefds = writefds;
		t_errorfds = errorfds;
		int result = select(0, &t_readfds, &t_writefds, &t_errorfds, &timeout);
		if (result == 0) continue;
		for (unordered_map<int, Handler *>::iterator iter = handlers.begin(); iter != handlers.end(); iter++) {
			int socket = iter->first;
			Handler *handler = iter->second;
			if (FD_ISSET(socket, &t_errorfds)) {
				handler->handle(socket);
			}
			if (FD_ISSET(socket, &t_readfds)) {
				handler->handle(socket);
			}
			if (FD_ISSET(socket, &t_writefds)) {
				handler->handle(socket);
			}
			if (handler->getDestroy()) {
				unregisterSocket(socket);
				closesocket(socket);
				delete handler;
				break;
			}
		}
	}
}
