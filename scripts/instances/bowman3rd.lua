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

function beginInstance()
	addInstanceMap(108010100);
	addInstanceMap(108010101);
end

function playerDisconnect(playerId, isPartyLeader)
	finish(playerId);
end

function timerEnd(name, fromTimer)
	if name == instance_timer then
		if getInstancePlayerCount() > 0 then
			moveAllPlayers(100000000);
			removeAllInstancePlayers();
		end
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		finish(playerId);
	end
end

function finish(playerId)
	removeInstancePlayer(playerId);
	markForDelete();
end