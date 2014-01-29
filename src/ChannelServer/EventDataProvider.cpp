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
#include "EventDataProvider.hpp"
#include "InitializeCommon.hpp"
#include "Instance.hpp"
#include "Instances.hpp"
#include "Timer.hpp"
#include "TimeUtilities.hpp"
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>

EventDataProvider::EventDataProvider()
{
	m_variables = make_owned_ptr<Variables>();
}

auto EventDataProvider::loadData() -> void {
	loadEvents();
	loadInstances();
}

auto EventDataProvider::loadEvents() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Events... ";

	// Declarations go here for regular server events or whatever you want to put on a timer

	// Starts a new timer that runs every hour
	// Timer::Timer::create(bind(&namespace::func, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t(1));

	// Same, except runs a class function
	// Timer::Timer::create(bind(&class::func, class instance, parameters),
	// Timer::Id(Timer::Types::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t(1));

	std::cout << "DONE" << std::endl;
}

auto EventDataProvider::loadInstances() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Instances... ";

	// Most common intervals with boats
	const time_point_t &now = TimeUtilities::getNow();
	const seconds_t &nearestFive = TimeUtilities::getDistanceToNextMinuteMark(5, now);
	const seconds_t &nearestTen = TimeUtilities::getDistanceToNextMinuteMark(10, now);
	const seconds_t &nearestFifteen = TimeUtilities::getDistanceToNextMinuteMark(15, now);

	startInstance("kerningToNlcBoarding", nearestFive, minutes_t(5));
	startInstance("nlcToKerningBoarding", nearestFive, minutes_t(5));

	std::cout << "DONE" << std::endl;
}

auto EventDataProvider::startInstance(const string_t &name, const duration_t &time, const duration_t &repeat) -> void {
	Instance *instance = new Instance(name, 0, 0, time, repeat, false, true);
	Instances::getInstance().addInstance(instance);
	instance->sendMessage(InstanceMessage::BeginInstance);
}