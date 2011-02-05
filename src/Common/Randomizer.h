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
#pragma once

#pragma warning(push)
#pragma warning(disable : 4146)
#pragma warning(disable : 4800)
#pragma warning(disable : 4996)
#include "MersenneTwister.h"
#pragma warning(pop)

#include "Types.h"
#include <string>
#include <boost/utility.hpp>

using std::string;

class MTRand;

class Randomizer : boost::noncopyable {
public:
	static Randomizer * Instance() {
		if (singleton == nullptr)
			singleton = new Randomizer;
		return singleton;
	}

	uint32_t randInt();
	uint32_t randInt(uint32_t max, uint32_t min = 0);
	uint16_t randShort(uint16_t max, uint16_t min = 0);
	uint8_t randChar(uint8_t max, uint8_t min = 0);
	double rand();
	string generateSalt(size_t length);
private:
	Randomizer() {};
	static Randomizer *singleton;

	MTRand mtrand;
};
