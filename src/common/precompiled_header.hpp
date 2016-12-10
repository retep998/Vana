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

// No need for header guard, this precompiled header file will never
//	be included twice.

#include <array>
#include <algorithm>
#include <asio.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
// Dependencies, can't do alphabetical order here
#include "common/types.hpp"
#include "common/maple_version.hpp"
#include "common/constant/alliance.hpp"
#include "common/constant/buddy.hpp"
#include "common/constant/buff.hpp"
#include "common/constant/character.hpp"
#include "common/constant/equip_slot.hpp"
#include "common/constant/gender.hpp"
#include "common/constant/guild.hpp"
#include "common/constant/inventory.hpp"
#include "common/constant/item.hpp"
#include "common/constant/map.hpp"
#include "common/constant/mob.hpp"
#include "common/constant/mob_skill.hpp"
#include "common/constant/monster_card.hpp"
#include "common/constant/party.hpp"
#include "common/constant/ship_kind.hpp"
#include "common/constant/skill.hpp"
#include "common/constant/stat.hpp"
#include "common/constant/status_effect.hpp"
#include "common/constant/item/flag.hpp"
#include "common/constant/item/max_stat.hpp"
#include "common/constant/item/scroll_type.hpp"
#include "common/constant/item/type.hpp"
#include "common/constant/item/cash/flag.hpp"
#include "common/constant/item/cash/serial.hpp"
#include "common/constant/job/id.hpp"
#include "common/constant/job/line.hpp"
#include "common/constant/job/progression.hpp"
#include "common/constant/job/track.hpp"
#include "common/constant/job/beginner_jobs.hpp"
#include "common/io/database.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/game_logic/job.hpp"
#include "common/util/game_logic/map.hpp"
#include "common/util/game_logic/mob_skill.hpp"
#include "common/util/game_logic/monster_card.hpp"
#include "common/util/game_logic/party.hpp"
#include "common/util/game_logic/player.hpp"
#include "common/util/game_logic/player_skill.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/string.hpp"
