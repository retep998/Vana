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

dofile("scripts/utils/miniDungeonHelper.lua");

function beginInstance()
	initMiniDungeonMaps(260020630, 35, 260020600, 261000000);
end

function timerEnd(name, fromTimer)
	miniDungeonTimerEnd(name, fromTimer);
end

function playerDisconnect(playerId, isPartyLeader)
	miniDungeonPlayerDisconnect(playerId, isPartyLeader);
end

function partyDisband(partyId)
	miniDungeonPartyDisband(partyId);
end

function partyRemoveMember(partyId, playerId)
	miniDunegonPartyRemoveMember(partyId, playerId);
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	miniDungeonChangeMap(playerId, newMap, oldMap, isPartyLeader);
end