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

dofile("scripts/lua_functions/boatHelper.lua");

function beginInstance()
	initBoatMaps({
		200090030,
		200090032,
		200090034,
		200090036,
		200090038,
		200090040,
		200090042,
		200090044,
		200090046,
		200090048,
		200090050,
		200090052,
		200090054,
		200090056,
		200090058,
	});
end

function timerEnd(name, fromTimer)
	if fromTimer then
		idx = name:find("done_");
		if idx ~= nil then
			playerId = tonumber(name:sub(6));
			if setPlayer(playerId) then
				setMap(130000210);
				revertPlayer(playerId);
			end
		end
	end
end

function playerDisconnect(playerId, isPartyLeader)
	if setPlayer(playerId) then
		cleanMap(playerId, getMap());
		revertPlayer();
	end
end

function cleanMap(playerId, mapId)
	stopInstanceTimer("done_" .. playerId);
	clearBoatMap(mapId);
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		removeInstancePlayer(playerId);
		cleanMap(playerId, oldMap);
	elseif not isInstanceMap(oldMap) then
		boatTime = 2 * 60;
		addInstancePlayer(playerId);
		showMapTimer(newMap, boatTime);
		startInstanceTimer("done_" .. playerId, boatTime);
		takeBoatMap(newMap);
	end
end