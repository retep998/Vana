/*
Copyright (C) 2008 Vana Development Team

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
#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#pragma warning(push)
#pragma warning(disable : 4146)
#pragma warning(disable : 4800)
#pragma warning(disable : 4996)
#include "MersenneTwister.h"
#pragma warning(pop) 
class MTRand;

class Randomizer {
public:
	static Randomizer * Instance() {
		if (singleton == 0)
			singleton = new Randomizer;
		return singleton;
	}

	int randInt(int max = 0);
	double rand();
	char * generateSalt(size_t length);
private:
	Randomizer() {};
	Randomizer(const Randomizer&);
	Randomizer& operator=(const Randomizer&);
	static Randomizer *singleton;

	MTRand mtrand;
};

#endif