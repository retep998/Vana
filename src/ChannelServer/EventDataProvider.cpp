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
#include "ChannelServer.hpp"
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
	// Timer::Id(TimerType::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t{1});

	// Same, except runs a class function
	// Timer::Timer::create(bind(&class::func, class instance, parameters),
	// Timer::Id(TimerType::EventTimer, ??, ??),
	// getTimers(), Timer::Time::getNthSecondOfHour(0), hours_t{1});

	std::cout << "DONE" << std::endl;
}

auto EventDataProvider::loadInstances() -> void {
	clearInstances();

	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Instances... ";

	// Most common intervals with boats
	time_point_t now = TimeUtilities::getNow();
	seconds_t nearestFour = TimeUtilities::getDistanceToNextMinuteMark(4, now);
	seconds_t nearestFive = TimeUtilities::getDistanceToNextMinuteMark(5, now);
	seconds_t nearestTen = TimeUtilities::getDistanceToNextMinuteMark(10, now);
	seconds_t nearestFifteen = TimeUtilities::getDistanceToNextMinuteMark(15, now);

	startInstance("ludiToKftBoarding", nearestFour, minutes_t{4});
	startInstance("kftToLudiBoarding", nearestFour, minutes_t{4});

	startInstance("kerningToNlcBoarding", nearestFive, minutes_t{5});
	startInstance("nlcToKerningBoarding", nearestFive, minutes_t{5});

	startInstance("kerningToCbdBoarding", nearestFive, minutes_t{5});
	startInstance("cbdToKerningBoarding", nearestFive, minutes_t{5});

	startInstance("elliniaToOrbisBoarding", nearestFifteen, minutes_t{15});
	startInstance("orbisToElliniaBoarding", nearestFifteen, minutes_t{15});

	startInstance("ludiToOrbisBoarding", nearestTen, minutes_t{10});
	startInstance("orbisToLudiBoarding", nearestTen, minutes_t{10});

	startInstance("leafreToOrbisBoarding", nearestTen, minutes_t{10});
	startInstance("orbisToLeafreBoarding", nearestTen, minutes_t{10});

	startInstance("ariantToOrbisBoarding", nearestTen, minutes_t{10});
	startInstance("orbisToAriantBoarding", nearestTen, minutes_t{10});

	init_list_t<string_t> timelessInstances = {
		"ereveToElliniaTrip", "elliniaToEreveTrip",
		"ereveToOrbisTrip", "orbisToEreveTrip",
		"miniDungeonPig",
		"miniDungeonGolem",
		"miniDungeonRabbit",
		"miniDungeonSand",
		"miniDungeonProtect",
		"miniDungeonRemember",
		"miniDungeonError",
		"miniDungeonMushroom",
		"miniDungeonRoundTable",
	};

	for (const auto &inst : timelessInstances) {
		startInstance(inst, seconds_t{0});
	}

	std::cout << "DONE" << std::endl;
}

auto EventDataProvider::clearInstances() -> void {
	auto clearInstance = [](const string_t &name) {
		if (Instance *instance = ChannelServer::getInstance().getInstances().getInstance(name)) {
			instance->markForDelete();
			ChannelServer::getInstance().getInstances().removeInstance(instance);
		}
	};

	auto instances = {
		"kerningToNlcBoarding", "nlcToKerningBoarding",
		"kerningToCbdBoarding", "cbdToKerningBoarding",
		"elliniaToOrbisBoarding", "orbisToElliniaBoarding",
		"ludiToOrbisBoarding", "orbisToLudiBoarding",
		"leafreToOrbisBoarding", "orbisToLeafreBoarding",
		"ariantToOrbisBoarding", "orbisToAriantBoarding",
		"ludiToKftBoarding", "kftToLudiBoarding",
		"ereveToElliniaTrip", "elliniaToEreveTrip",
		"ereveToOrbisTrip", "orbisToEreveTrip",
		"miniDungeonPig",
		"miniDungeonGolem",
		"miniDungeonRabbit",
		"miniDungeonSand",
		"miniDungeonProtect",
		"miniDungeonRemember",
		"miniDungeonError",
		"miniDungeonMushroom",
		"miniDungeonRoundTable",
	};

	for (const auto &instance : instances) {
		clearInstance(instance);
	}
}

auto EventDataProvider::startInstance(const string_t &name, const duration_t &time, const duration_t &repeat) -> void {
	Instance *instance = new Instance(name, 0, 0, time, repeat, false);
	ChannelServer::getInstance().getInstances().addInstance(instance);
	instance->beginInstance();
}