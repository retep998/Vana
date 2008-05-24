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
#ifndef CONNECTION_H
#define CONNECTION_H

#include "Selector.h"
#include "AbstractPlayer.h"
#include "MapleSocket.h"
#include <SocketHandler.h>

class Connection {
public:
	static Connection * Instance() {
		if (singleton == 0)
			singleton = new Connection;
		return singleton;
	}

	~Connection() { terminate = true; }

	void accept(short port, AbstractPlayerFactory *apf);
	AbstractPlayer * connect(const char *ip, short port, AbstractPlayerFactory *apf);
	void startSelect();
	void select();

	static void _select(Connection *obj);
private:
	Connection();
	Connection(const Connection&);
	Connection& operator=(const Connection&);
	static Connection *singleton;

	bool terminate;
	bool selecting;
	SocketHandler h;
};

#endif