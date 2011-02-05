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

#include "Timer/Container.h"
#include "Types.h"
#include "Variables.h"
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;

namespace Timer {
	struct Id;
	class Container;
};

class EventDataProvider : boost::noncopyable {
public:
	static EventDataProvider * InstancePtr() {
		if (singleton == nullptr)
			singleton = new EventDataProvider;
		return singleton;
	}

	void loadData();
	Timer::Container * getTimers() const { return m_timers.get(); }
	Variables * getVariables() const { return m_variables.get(); }
private:
	EventDataProvider();
	static EventDataProvider *singleton;

	void loadEvents();
	void loadInstances();
	void startInstance(const string &name, int32_t time, int32_t repeat = 0);

	boost::scoped_ptr<Timer::Container> m_timers;
	boost::scoped_ptr<Variables> m_variables;
};
