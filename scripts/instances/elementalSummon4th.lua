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
	addInstanceMap(922020100);
end

function playerDisconnect(playerId, isPartyLeader)
	finish(playerId);
end

function instanceTimerEnd(fromTimer)
	if getInstancePlayerCount() > 0 then
		finish2();
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		finish(playerId);
	end
end

function partyDisband(partyId)
	finish2();
end

function partyRemoveMember(partyId, playerId)
	finish2();
end

function finish(playerId)
	removeInstancePlayer(playerId);
	finish2();
	markForDelete();
end

function finish2()
	moveAllPlayers(220050300);
	removeAllInstancePlayers();
end