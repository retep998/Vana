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
#include "common_temp/Types.hpp"
#include "common_temp/MapleVersion.hpp"
#include "common_temp/GameConstants.hpp"
#include "common_temp/JobConstants.hpp"
#include "common_temp/SkillConstants.hpp"
#include "common_temp/GameLogicUtilities.hpp"
#include "common_temp/PacketBuilder.hpp"
#include "common_temp/PacketReader.hpp"
#include "common_temp/Randomizer.hpp"
#include "common_temp/Database.hpp"
#include "common_temp/StringUtilities.hpp"
