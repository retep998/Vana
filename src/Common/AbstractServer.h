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
#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include "Ip.h"
#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class AbstractServer {
public:
	virtual ~AbstractServer() { }
	
	void initialize();
	virtual void listen() = 0;
	virtual void loadConfig() = 0;
	virtual void loadData() = 0;
	virtual void shutdown();
	
	clock_t getStartTime() const { return startTime; }
	string getInterPassword() const { return inter_password; }
	void displayLaunchTime() const;
protected:
	AbstractServer();

	clock_t startTime;
	bool to_listen;
	string inter_password;
	IpMatrix external_ip;
};

#endif
