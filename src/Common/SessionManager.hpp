/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Types.hpp"
#include <memory>
#include <unordered_set>

class AbstractSession;

class SessionManager {
	NONCOPYABLE(SessionManager);
public:
	SessionManager() = default;
	auto start(ref_ptr_t<AbstractSession> session) -> void;
	auto stop(ref_ptr_t<AbstractSession> session) -> void;
	auto stopAll() -> void;
private:
	hash_set_t<ref_ptr_t<AbstractSession>> m_sessions;
};