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
	addInstanceMap(220080001);
	players = getAllMapPlayerIds(220080001);
	for i = 1, #players do
		addInstancePlayer(players[i]);
	end
end

function playerDisconnect(playerId, isPartyLeader)
	cleanUpPap();
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		-- Player probably died, want to make sure this doesn't keep the room full
		removeInstancePlayer(playerId);
		cleanUpPap();
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

function playerDeath(playerId) end
function instanceTimerEnd(fromTimer) end
function timerEnd(name, fromTimer) end
function mobDeath(mobId, mapMobId, mapId) end
function mobSpawn(mobId, mapMobId, mapId) end
function friendlyHit(mobId, mapMobId, mapId, hp, maxHp) end
function partyDisband(partyId) end
function partyRemoveMember(partyId, playerId) end