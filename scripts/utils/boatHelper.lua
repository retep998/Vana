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
-- A subset of Lua functions that helps with common boat patterns

function getInstanceMinutes()
	local g = getInstanceTime();
	g = (g - (g % 60)) / 60 + 1;
	return g;
end

function doElevatorDoorCheck(mapId)
	local state = nil;
	if getInstanceMinutes() <= 1 then
		state = 0;
	else
		state = 1;
	end

	if getReactorState(mapId, 2208004) ~= state then
		setReactorState(mapId, 2208004, state);
	end
end

function doBoatDockCheck(mapId)
	local docked = nil;
	if getInstanceMinutes() <= (getInstanceVariable("boat_time", type_int) - 5) then
		docked = boat_docked;
	else
		docked = boat_undocked;
	end

	setBoatDocked(mapId, docked);
end