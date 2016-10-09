--[[
Copyright (C) 2008-2016 Vana Development Team

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
--]]
-- This file is designed to show you all the messages you can possibly use
-- Also a short description of the message and parameters for easy reference

function beginInstance()
	-- Message is sent the instant that you call createInstance
end

function playerDeath(playerId)
	-- Happens every time a player dies

	-- playerId: The ID of the player that died
end

function playerDisconnect(playerId, isPartyLeader)
	-- Happens every time a player disconnects
	-- Note that this occurs *after* the disconnecting player is removed from the map
	-- As such, getNumPlayers(mapId) will be 0 if they were the last player on the map

	-- playerId: The ID of the player that disconnected
	-- isPartyLeader: A boolean representing whether or not the player was a party leader
end

function timerEnd(name, fromTimer)
	-- Happens every time a timer started by startInstanceFutureTimer or startInstanceSecondOfHourTimer ends
	-- All instances have an implicit timer with the name "instance" (if the instance has an instance time)
	-- This will *not* be triggered for the instance timer in the case where you restart or stop the instance timer manually

	-- name: A string indicating the identifier of the timer
	-- fromTimer: A boolean indicating whether the timer ended prematurely or not
end

function mobDeath(mobId, mapMobId, mapId)
	-- Happens every time a mob dies

	-- mobId: The mob template ID (e.g. Snail is 100100)
	-- mapMobId: The map-specific ID for a particular instance of the mob
	-- mapId: The ID of the map where the mob died
end

function mobSpawn(mobId, mapMobId, mapId)
	-- Happens every time a mob is spawned

	-- mobId: The mob template ID (e.g. Snail is 100100)
	-- mapMobId: The map-specific ID for a particular instance of the mob
	-- mapId: The ID of the map where the mob spawned
end

function friendlyHit(mobId, mapMobId, mapId, hp, maxHp)
	-- Happens every time a friendly mob is damaged (e.g. Tylus in El Nath PQ)

	-- mobId: The mob template ID (e.g. Snail is 100100)
	-- mapMobId: The map-specific ID for a particular instance of the mob
	-- mapId: The ID of the map where the friendly mob was damaged
	-- hp: The current HP level of the friendly mob
	-- maxHp: The maximum HP level of the friendly mob
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	-- Happens every time a player switches to or from an instance map
	-- Players that are not in an instance will trigger this as well
	-- Note that this occurs *before* the player is removed from the old map
	-- As such, getNumPlayers(newMap) will be 0 if they are the first on the map
	-- Additionally, getNumPlayers(oldMap) will be 1 if they are the last on the map

	-- playerId: The ID of the player that changed maps
	-- newMap: The map ID of the destination map
	-- oldMap: The map ID of the source map
	-- isPartyLeader: A boolean representing whether or not the player was a party leader
end

function partyDisband(partyId)
	-- Happens when a party added to an instance disbands
	-- This occurs prior to any removal of players/etc. so all party exports should work as expected

	-- partyId: The ID of the party that disbanded
end

function partyRemoveMember(partyId, playerId)
	-- Happens when a party added to an instance kicks a member or a member leaves
	-- This occurs prior to any removal of the player so all party exports should work as expected

	-- partyId: The ID of the party that lost a member
	-- playerId: The ID of the player that was removed
end