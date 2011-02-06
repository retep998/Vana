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

function beginInstance()
	addInstanceMap(108010300);
	addInstanceMap(108010301);
end

function playerDisconnect(playerid, ispartyleader)
	finish(playerid);
end

function instanceTimerEnd(fromtimer)
	if getInstancePlayerCount() > 0 then
		moveAllPlayers(102000000);
		removeAllInstancePlayers();
	end
end

function changeMap(playerid, newmap, oldmap, ispartyleader)
	if not isInstanceMap(newmap) then
		finish(playerid);
	end
end

function finish(playerid)
	removeInstancePlayer(playerid);
	markForDelete();
end

function playerDeath(playerid) end
function timerEnd(name, fromtimer) end
function mobDeath(mobid, mapmobid, mapid) end
function mobSpawn(mobid, mapmobid, mapid) end
function friendlyHit(mobid, mapmobid, mapid, hp, maxhp) end
function partyDisband(partyid) end
function partyRemoveMember(partyid, playerid) end
