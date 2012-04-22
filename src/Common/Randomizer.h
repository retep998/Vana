/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "noncopyable.hpp"
#include "Types.h"
#include <limits>
#include <random>
#include <string>

using std::string;
using std::mt19937;
using std::uniform_int;

class Randomizer : boost::noncopyable {
public:
	static Randomizer * Instance() {
		if (singleton == nullptr)
			singleton = new Randomizer;
		return singleton;
	}

	uint32_t randInt(uint32_t max, uint32_t min = 0);
	uint32_t randInt();
	uint16_t randShort(uint16_t max, uint16_t min = 0);
	uint8_t randChar(uint8_t max, uint8_t min = 0);
	string generateSalt(size_t length);
private:
	Randomizer() {
		m_engine.seed(std::rand());
		m_distribution = uniform_int<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
	}
	static Randomizer *singleton;

	mt19937 m_engine;
	uniform_int<uint32_t> m_distribution;
};