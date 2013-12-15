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

#include "TimerId.h"
#include "TimerTypes.h"
#include "Types.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace Timer {

class Timer;

using std::shared_ptr;
using std::unordered_map;

class Container {
public:
	seconds_t getSecondsRemaining(const Id &id);
	milliseconds_t getMillisecondsRemaining(const Id &id);
	bool isTimerRunning(const Id &id);
	void registerTimer(shared_ptr<Timer> timer);
	void removeTimer(const Id &id);
private:
	unordered_map<Id, shared_ptr<Timer>, std::hash<Id>> m_timers;
};

}