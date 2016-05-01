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
	addInstanceMap(220080001);
	players = getAllMapPlayerIds(220080001);
	for i = 1, #players do
		addInstancePlayer(players[i]);
	end
end

function playerDisconnect(playerId, isPartyLeader)
	cleanUpPapulatus();
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		-- Player probably died, want to make sure this doesn't keep the room full
		removeInstancePlayer(playerId);
		cleanUpPapulatus();
	end
end

function cleanUpPapulatus()
	if getInstancePlayerCount() == 0 then
		setReactorState(220080000, 2208001, 0);
		setReactorState(220080000, 2208003, 0);
		setReactorState(220080001, 2201004, 0);
		markForDelete();
	end
end