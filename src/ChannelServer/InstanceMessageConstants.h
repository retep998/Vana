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

enum InstanceMessages {
	// 5 parameters
	FriendlyMobHit, // Mob ID, map mob ID, map ID, HP, max HP

	// 4 parameters
	PlayerChangeMap, // Player ID, new map ID, old map ID, is party leader

	// 3 parameters
	MobDeath, // Mob ID, map mob ID, map ID
	MobSpawn, // Mob ID, map mob ID, map ID

	// 2 parameters
	TimerEnd, // Timer name, boolean false
	TimerNaturalEnd, // Timer name, boolean true
	PartyRemoveMember, // Party ID, Player ID
	PlayerDisconnect, // Player ID, is party leader

	// 1 parameter
	PlayerDeath, // Player ID
	InstanceTimerEnd, // Boolean false
	InstanceTimerNaturalEnd, // Boolean true
	PartyDisband, // Party ID

	// 0 parameters
	BeginInstance
};
