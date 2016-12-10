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
#pragma once

#include "common/file_time.hpp"
#include "common/types.hpp"
#include <limits>

namespace vana {
	namespace constant {
		namespace inventory {
			const game_inventory count = 5;
			const game_inventory equip = 1;
			const game_inventory use = 2;
			const game_inventory setup = 3;
			const game_inventory etc = 4;
			const game_inventory cash = 5;

			const game_inventory_slot_count min_slots_per_inventory = 24;
			const game_inventory_slot_count max_slots_per_inventory = 100;
			const game_storage_slot min_slots_storage = 4;
			const game_storage_slot max_slots_storage = 100;

			const uint8_t equipped_slots = 51;
			const int8_t max_pet_count = 3;
			const uint8_t vip_rock_max = 10;
			const uint8_t teleport_rock_max = 5;

			const game_mesos min_drop_mesos = 10;
			const game_mesos max_drop_mesos = 50000;
		}
	}
}