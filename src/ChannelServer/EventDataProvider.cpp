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
#include "TimeUtilities.h"
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>

using std::tr1::bind;
using Initializing::outputWidth;

EventDataProvider * EventDataProvider::singleton = nullptr;

EventDataProvider::EventDataProvider() :
m_timers(new Timer::Container),
m_variables(new Variables)
{
}

void EventDataProvider::loadData() {
	loadEvents();
	loadInstances();
}

void EventDataProvider::loadEvents() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Events... ";

	// Declarations go here for regular server events or whatever you want to put on a timer

	// Starts a new timer that runs every hour
	// new Timer::Timer(bind(&namespace::func, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), 60 * 60 * 1000);

	// Same, except runs a class function
	// new Timer::Timer(bind(&class::func, class instance, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), 60 * 60 * 1000);

	std::cout << "DONE" << std::endl;
}

void EventDataProvider::loadInstances() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Instances... ";

	int32_t nearestTen = TimeUtilities::getNearestMinuteMark(10); // Most common intervals with boats
	int32_t nearestFifteen = TimeUtilities::getNearestMinuteMark(15);

	startInstance("kerningToNlcBoarding", -nearestTen, 10 * 60);
	startInstance("nlcToKerningBoarding", -nearestTen, 10 * 60);

	std::cout << "DONE" << std::endl;
}

void EventDataProvider::startInstance(const string &name, int32_t time, int32_t repeat) {
	Instance *instance = new Instance(name, 0, 0, time, repeat, false, true);
	Instances::InstancePtr()->addInstance(instance);
	instance->sendMessage(BeginInstance);
}