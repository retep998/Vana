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

function playerDeath(playerid)
	-- Happens every time a player dies
end

function playerDisconnect(playerid, ispartyleader)
	-- Happens every time a player disconnects
end

function instanceTimerEnd(fromtimer)
	-- Happens when the instance timer ends
end

function timerEnd(name, fromtimer)
	-- Happens every time a timer started by startInstanceTimer ends
end

function mobDeath(mobid, mapmobid, mapid)
	-- Happens every time a mob dies
end

function mobSpawn(mobid, mapmobid, mapid)
	-- Happens every time a mob is spawned
end

function friendlyHit(mobid, mapmobid, mapid, hp, maxhp)
	-- Happens every time a friendly mob is damaged
end

function changeMap(playerid, newmap, oldmap, ispartyleader)
	-- Happens every time a player switches to or from an instance map
	-- Players need not be a part of the instance to trigger this
end

function partyDisband(partyid)
	-- Happens when a party added to an instance disbands
end

function partyRemoveMember(partyid, playerid)
	-- Happens when a party added to an instance boots a member or one leaves
end