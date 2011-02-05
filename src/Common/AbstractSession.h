/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

class SessionManager;
typedef std::tr1::shared_ptr<SessionManager> SessionManagerPtr;

class AbstractSession {
public:
	virtual ~AbstractSession() { }

	AbstractSession(SessionManagerPtr sessionManager) :
		m_sessionManager(sessionManager) { }
	virtual void start() = 0;
	virtual void handle_start() = 0;
	virtual void stop() = 0;
	virtual void disconnect() = 0;
	virtual void handle_stop() = 0;
protected:
	SessionManagerPtr m_sessionManager;
};

typedef boost::shared_ptr<AbstractSession> AbstractSessionPtr;
