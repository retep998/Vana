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
#include "common/types_temp.hpp"
#include "common/maple_version.hpp"
#include "common/game_constants.hpp"
#include "common/job_constants.hpp"
#include "common/skill_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/randomizer_temp.hpp"
#include "common/database_temp.hpp"
#include "common/string_utilities.hpp"
