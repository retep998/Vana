--[[
Copyright (C) 2008-2015 Vana Development Team

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

dofile("scripts/utils/mapManagerHelper.lua");

function beginInstance()
	initManagedMaps({
		200090020,
		200090022,
		200090024,
		200090026,
		200090028,
	});
end

function timerEnd(name, fromTimer)
	if fromTimer then
		local playerId = getIdFromManagedMapTimer(name);
		if playerId and setPlayer(playerId) then
			setMap(130000210);
			revertPlayer(playerId);
		end
	end
end

function playerDisconnect(playerId, isPartyLeader)
	if setPlayer(playerId) then
		cleanManagedMapInstance(playerId, getMap());
		revertPlayer();
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		removeInstancePlayer(playerId);
		cleanManagedMapInstance(playerId, oldMap);
	elseif not isInstanceMap(oldMap) then
		boatTime = 8 * 60;
		addInstancePlayer(playerId);
		startManagedMapInstance(playerId, newMap, boatTime);
	end
end