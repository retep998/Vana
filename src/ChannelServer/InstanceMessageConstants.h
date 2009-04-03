/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef INSTANCEMESSAGECONSTANTS_H
#define INSTANCEMESSAGECONSTANTS_H

enum InstanceMessages {
	// 3 parameters
	PLAYER_CHANGEMAP, // Player ID, new map ID, old map ID

	// 2 parameters
	MOB_DEATH, // Mob ID, map mob ID
	MOB_SPAWN, // Mob ID, map mob ID
	TIMER_END, // Timer name, boolean false
	TIMER_NATURAL_END, // Timer name, boolean true

	// 1 parameter
	PLAYER_DEATH, // Player ID
	PLAYER_DC, // Player ID
	INSTANCETIMER_END, // Boolean false
	INSTANCETIMER_NATURAL_END, // Boolean true

	// 0 parameters
	BEGIN_INSTANCE
};

#endif