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
#ifndef PETSPACKET_H
#define PETSPACKET_H

#include "Types.h"
#include <string>

using std::string;

class Player;
class Pet;

namespace PetsPacket {
	void showChat(Player *player, Pet *pet, const string &message, int8_t act);
	void movePet(Player *player, Pet *pet, unsigned char *buf, int32_t buflen);
	void petSummoned(Player *player, Pet *pet, bool kick = false, Player *showTo = 0);
	void showAnimation(Player *player, Pet *pet, int8_t animation, bool success = false);
	void updatePet(Player *player, Pet *pet);
	void levelUp(Player *player, Pet *pet);
	void changeName(Player *player, Pet *pet);
	void showPet(Player *player, Pet *pet);
	void updateSummonedPets(Player *player);
	void blankUpdate(Player *player);
};

#endif
