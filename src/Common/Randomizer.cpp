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
#include "Randomizer.h"
#include <boost/scoped_array.hpp>

Randomizer * Randomizer::singleton = 0;

int Randomizer::randInt(int max) {
	return mtrand.randInt(max);
}

int Randomizer::randInt() {
	return mtrand.randInt();
}

double Randomizer::rand() {
	return mtrand.rand();
}

string Randomizer::generateSalt(size_t length) {
	boost::scoped_array<char> salt(new char[length+1]);
	for (size_t i = 0; i < length; i++) {
		salt[i] = 33 + randInt(93);
	}
	salt[length] = 0;
	return string(salt.get());
}
