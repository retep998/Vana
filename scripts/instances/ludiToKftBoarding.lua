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

dofile("scripts/utils/boatHelper.lua");

function beginInstance()
	addInstanceMap(222020210);
	doElevatorDoorCheck(222020200);
	startInstanceTimer("door_check", getNearestMinute(1), 60);
end

function timerEnd(name, fromTimer)
	if fromTimer then
		if name == "door_check" then
			doElevatorDoorCheck(222020200);
		end
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if isInstanceMap(newMap) then
		addInstancePlayer(playerId);
	elseif not isInstanceMap(oldMap) then
		removeInstancePlayer(playerId);
	end
end

function instanceTimerEnd(fromTimer)
	if getInstancePlayerCount() > 0 then
		createInstance("ludiToKftTrip", 60, false);
		passPlayersBetweenInstances(222020211);
	end
end