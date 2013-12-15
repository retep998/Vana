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
#include "EventDataProvider.h"
#include "InitializeCommon.h"
#include "Instance.h"
#include "Instances.h"
#include "Timer.h"
#include "TimeUtilities.h"
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>

using std::bind;
using Initializing::OutputWidth;

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
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Events... ";

	// Declarations go here for regular server events or whatever you want to put on a timer

	// Starts a new timer that runs every hour
	// Timer::create(bind(&namespace::func, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t(1));

	// Same, except runs a class function
	// Timer::create(bind(&class::func, class instance, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t(1));

	std::cout << "DONE" << std::endl;
}

void EventDataProvider::loadInstances() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Instances... ";

	// Most common intervals with boats
	const time_point_t &now = TimeUtilities::getNow();
	const seconds_t &nearestTen = -TimeUtilities::getDistanceToNextMinuteMark(10,  now);
	const seconds_t &nearestFifteen = -TimeUtilities::getDistanceToNextMinuteMark(15,  now);

	//startInstance("kerningToNlcBoarding", nearestTen, minutes_t(10));
	//startInstance("nlcToKerningBoarding", nearestTen, minutes_t(10));

	std::cout << "DONE" << std::endl;
}

void EventDataProvider::startInstance(const string &name, const seconds_t &time, const seconds_t &repeat) {
	Instance *instance = new Instance(name, 0, 0, time, repeat, false, true);
	Instances::InstancePtr()->addInstance(instance);
	instance->sendMessage(BeginInstance);
}