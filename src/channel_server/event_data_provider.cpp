/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "event_data_provider.hpp"
#include "common/data/initialize.hpp"
#include "common/timer/timer.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/instances.hpp"
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>

namespace vana {
namespace channel_server {

event_data_provider::event_data_provider()
{
	m_variables = make_owned_ptr<variables>();
}

auto event_data_provider::load_data() -> void {
	load_events();
	load_instances();
}

auto event_data_provider::load_events() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Events... ";

	// Declarations go here for regular server events or whatever you want to put on a timer

	// Starts a new timer that runs every hour
	// vana::timer::timer::create(bind(&namespace::func, parameters),
	// vana::timer::id{timer_type::event_timer, ??, ??},
	// get_timers(), timer::time::get_nth_second_of_hour(0), hours{1});

	// Same, except runs a class function
	// vana::timer::timer::create(bind(&class::func, class instance, parameters),
	// vana::timer::id{timer_type::event_timer, ??, ??},
	// get_timers(), timer::time::get_nth_second_of_hour(0), hours{1});

	std::cout << "DONE" << std::endl;
}

auto event_data_provider::load_instances() -> void {
	clear_instances();

	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Instances... ";

	// Most common intervals with boats
	time_point now = vana::util::time::get_now();
	seconds nearest_four = vana::util::time::get_distance_to_next_minute_mark(4, now);
	seconds nearest_five = vana::util::time::get_distance_to_next_minute_mark(5, now);
	seconds nearest_ten = vana::util::time::get_distance_to_next_minute_mark(10, now);
	seconds nearest_fifteen = vana::util::time::get_distance_to_next_minute_mark(15, now);

	start_instance("ludiToKftBoarding", nearest_four, minutes{4});
	start_instance("kftToLudiBoarding", nearest_four, minutes{4});

	start_instance("kerningToNlcBoarding", nearest_five, minutes{5});
	start_instance("nlcToKerningBoarding", nearest_five, minutes{5});

	start_instance("kerningToCbdBoarding", nearest_five, minutes{5});
	start_instance("cbdToKerningBoarding", nearest_five, minutes{5});

	start_instance("elliniaToOrbisBoarding", nearest_fifteen, minutes{15});
	start_instance("orbisToElliniaBoarding", nearest_fifteen, minutes{15});

	start_instance("ludiToOrbisBoarding", nearest_ten, minutes{10});
	start_instance("orbisToLudiBoarding", nearest_ten, minutes{10});

	start_instance("leafreToOrbisBoarding", nearest_ten, minutes{10});
	start_instance("orbisToLeafreBoarding", nearest_ten, minutes{10});

	start_instance("ariantToOrbisBoarding", nearest_ten, minutes{10});
	start_instance("orbisToAriantBoarding", nearest_ten, minutes{10});

	init_list<string> timeless_instances = {
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

	for (const auto &inst : timeless_instances) {
		start_instance(inst, seconds{0});
	}

	std::cout << "DONE" << std::endl;
}

auto event_data_provider::clear_instances() -> void {
	auto clear_instance = [](const string &name) {
		if (instance *inst = channel_server::get_instance().get_instances().get_instance(name)) {
			inst->mark_for_delete();
			channel_server::get_instance().get_instances().remove_instance(inst);
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
		clear_instance(instance);
	}
}

auto event_data_provider::start_instance(const string &name, const duration &time, const duration &repeat) -> void {
	instance *inst = new instance{name, 0, 0, time, repeat, false};
	channel_server::get_instance().get_instances().add_instance(inst);
	inst->begin_instance();
}

}
}