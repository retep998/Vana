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
#include "Randomizer.h"

Randomizer * Randomizer::singleton = nullptr;

uint32_t Randomizer::randInt() {
	return mtrand.randInt();
}

uint32_t Randomizer::randInt(uint32_t max, uint32_t min) {
	uint32_t diff = max - min;
	return mtrand.randInt(diff) + min;
}

uint16_t Randomizer::randShort(uint16_t max, uint16_t min) {
	uint16_t diff = max - min;
	return static_cast<uint16_t>(mtrand.randInt(diff)) + min;
}

uint8_t Randomizer::randChar(uint8_t max, uint8_t min) {
	uint8_t diff = max - min;
	return static_cast<uint8_t>(mtrand.randInt(diff)) + min;
}

double Randomizer::rand() {
	return mtrand.rand();
}

string Randomizer::generateSalt(size_t length) {
	string salt(length, 0);
	for (size_t i = 0; i < length; i++) {
		salt[i] = randChar(126, 33);
	}
	return salt;
}
