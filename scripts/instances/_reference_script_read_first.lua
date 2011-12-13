--[[
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
--]]
-- This file is designed to show you all the messages you can possibly use
-- Also a short description of the message and parameters for easy reference

function beginInstance()
	-- Message is sent the instant that you call createInstance
end

function playerDeath(playerId)
	-- Happens every time a player dies
end

function playerDisconnect(playerId, isPartyLeader)
	-- Happens every time a player disconnects
end

function instanceTimerEnd(fromTimer)
	-- Happens when the instance timer ends
end

function timerEnd(name, fromTimer)
	-- Happens every time a timer started by startInstanceTimer ends
end

function mobDeath(mobId, mapMobId, mapId)
	-- Happens every time a mob dies
end

function mobSpawn(mobId, mapMobId, mapId)
	-- Happens every time a mob is spawned
end

function friendlyHit(mobId, mapMobId, mapId, hp, maxHp)
	-- Happens every time a friendly mob is damaged
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	-- Happens every time a player switches to or from an instance map
	-- Players need not be a part of the instance to trigger this
end

function partyDisband(partyId)
	-- Happens when a party added to an instance disbands
end

function partyRemoveMember(partyId, playerId)
	-- Happens when a party added to an instance boots a member or one leaves
end