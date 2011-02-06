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
#include "EventDataProvider.h"
#include "InitializeCommon.h"
#include "Instance.h"
#include "Instances.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>

using std::tr1::bind;

EventDataProvider * EventDataProvider::singleton = 0;

EventDataProvider::EventDataProvider() : m_timers(new Timer::Container), m_variables(new Variables) {

}

void EventDataProvider::loadEvents() {
	std::cout << std::setw(Initializing::outputWidth) << std::left << "Initializing Events... ";

	// Declarations go here for boats and regular server events

	// Starts a new timer that runs every hour
	// new Timer::Timer(bind(&namespace::func, parameter),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), 60 * 60 * 1000);
	std::cout << "DONE" << std::endl;

	std::cout << std::setw(Initializing::outputWidth) << std::left << "Initializing Instances... ";

	// Starts a new instance, boats or whatevers
	// Instance *instance = new Instance("scriptName.lua", 0, 0, -3600, false, false);
	// Instances::InstancePtr()->addInstance(instance);
	// instance->sendMessage(BeginInstance);

	std::cout << "DONE" << std::endl;
}