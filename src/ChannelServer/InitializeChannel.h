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
#ifndef INITIALIZECHANNEL_H
#define INITIALIZECHANNEL_H

#include "InitializeCommon.h"
#include "Mobs.h"
#include "Drops.h"
#include "Maps.h"
#include "NPCs.h"
#include "Shops.h"
#include <string>
#include "Quests.h"
#include "Skills.h"
#include "Inventory.h"

using namespace std;

namespace Initializing {
	void initializeMobs();
	void initializeDrops();
	void initializeMaps();
	void initializeEquips();
	void initializeShops();
	void initializeItems();
	void initializeQuests();
	void initializeSkills();
}

#endif
