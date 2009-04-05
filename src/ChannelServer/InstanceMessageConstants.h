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
	Player_Changemap, // Player ID, new map ID, old map ID

	// 2 parameters
	Mob_Death, // Mob ID, map mob ID
	Mob_Spawn, // Mob ID, map mob ID
	Timer_End, // Timer name, boolean false
	Timer_Natural_End, // Timer name, boolean true

	// 1 parameter
	Player_Death, // Player ID
	Player_Disconnect, // Player ID
	Instance_Timer_End, // Boolean false
	Instance_Timer_Natural_End, // Boolean true

	// 0 parameters
	Begin_Instance
};

#endif