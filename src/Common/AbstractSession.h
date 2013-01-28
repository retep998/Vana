/*
Copyright (C) 2008-2013 Vana Development Team

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

#include <memory>

class SessionManager;
typedef std::shared_ptr<SessionManager> SessionManagerPtr;

class AbstractSession {
public:
	friend class SessionManager;
	virtual ~AbstractSession() { }

	AbstractSession(SessionManagerPtr sessionManager, bool encrypted = true) :
		m_sessionManager(sessionManager),
		m_encrypt(encrypted)
	{
	}
	virtual void disconnect() = 0;
protected:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void handleStart() = 0;
	virtual void handleStop() = 0;
	bool isEncrypted() const { return m_encrypt; }
	void setEncrypted(bool encrypted) { m_encrypt = encrypted; }

	SessionManagerPtr m_sessionManager;
	bool m_encrypt;
};

typedef std::shared_ptr<AbstractSession> AbstractSessionPtr;