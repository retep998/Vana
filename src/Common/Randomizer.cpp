/*
Copyright (C) 2008-2013 Vana Development Team

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

Randomizer * Randomizer::singleton = nullptr;

uint32_t Randomizer::randInt(uint32_t max, uint32_t min) {
	uint32_t diff = (max - min) + 1;
	return (randInt() % diff) + min;
}

uint32_t Randomizer::randInt() {
	// TODO:
	// Look at alternate solutions for this, possibly setting min/max per distribution
	// I don't want to commit to using inline distribution objects because they have an internal state and that may break the RNG calculation
	// So instead for now, I just do a simple set of calculations on the result
	// It's probably less effective than the real generation, but I think it's okay for this purpose for now
	// Will take a better look in the future
	return m_distribution(m_engine);
}

uint16_t Randomizer::randShort(uint16_t max, uint16_t min) {
	return static_cast<uint16_t>(randInt(max, min));
}

uint8_t Randomizer::randChar(uint8_t max, uint8_t min) {
	return static_cast<uint8_t>(randInt(max, min));
}

string Randomizer::generateSalt(size_t length) {
	string salt(length, 0);
	for (size_t i = 0; i < length; i++) {
		salt[i] = randChar(126, 33);
	}
	return salt;
}