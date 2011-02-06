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
	addInstanceMap(220080001);
	players = getAllMapPlayerIDs(220080001);
	for i = 1, #players do
		addInstancePlayer(players[i]);
	end
end

function playerDisconnect(playerid, ispartyleader)
	startInstanceTimer("clean", 1, false);
end

function timerEnd(name, fromtimer)
	cleanUpPap();
end

function changeMap(playerid, newmap, oldmap, ispartyleader)
	if not isInstanceMap(newmap) then -- Player probably died, want to make sure this doesn't keep the room full
		removeInstancePlayer(playerid);
		startInstanceTimer("clean", 1, false);
	end
end

function cleanUpPap()
	if getInstancePlayerCount() == 0 then
		setReactorState(220080000, 2208001, 0);
		setReactorState(220080000, 2208003, 0);
		setReactorState(220080001, 2201004, 0);
		markForDelete();
	end
end

function playerDeath(playerid) end
function instanceTimerEnd(fromtimer) end
function mobDeath(mobid, mapmobid, mapid) end
function mobSpawn(mobid, mapmobid, mapid) end
function friendlyHit(mobid, mapmobid, mapid, hp, maxhp) end
function partyDisband(partyid) end
function partyRemoveMember(partyid, playerid) end