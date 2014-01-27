--[[
Copyright (C) 2008-2014 Vana Development Team

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
	addInstanceMap(600010003);
end

function playerDisconnect(playerId, isPartyLeader)
	finish();
end

function instanceTimerEnd(fromTimer)
	if getInstancePlayerCount() > 0 then
   		moveAllPlayers(103000100);
		removeAllInstancePlayers();
  	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		finish();
		removeInstancePlayer(playerId);
	else
		addInstancePlayer(playerId);
	end
end

function finish()
	if getInstancePlayerCount() == 1 then
		markForDelete();
	end
end

function playerDeath(playerId) end
function timerEnd(name, fromTimer) end
function mobDeath(mobId, mapMobId, mapId) end
function mobSpawn(mobId, mapMobId, mapId) end
function friendlyHit(mobId, mapMobId, mapId, hp, maxHp) end
function partyDisband(partyId) end
function partyRemoveMember(partyId, playerId) end