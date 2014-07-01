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
-- A subset of Lua functions that helps with common managing map patterns (e.g. boats and minidungeons)

function initManagedMaps(maps)
	local temp = "";
	for i = 1, #maps do
		local mapId = maps[i];
		addInstanceMap(mapId);

		if #temp > 0 then
			temp = temp .. ",";
		end
		temp = temp .. mapId;
	end

	setInstanceVariable("available_maps", temp);
end

function queryManagedMap(instanceName)
	local temp = nil;
	if setInstance(instanceName) then
		local mapIds = getInstanceVariable("available_maps");
		if mapIds ~= nil and #mapIds > 0 then
			for map in mapIds:gmatch("([^,]+)") do
				temp = tonumber(map);
				break;
			end
		end

		revertInstance();
	end

	return temp;
end

function takeManagedMap(mapId)
	local mapIds = getInstanceVariable("available_maps");
	if mapIds == nil then
		mapIds = "";
	end

	local temp = "";
	for map in mapIds:gmatch("([^,]+)") do
		map = tonumber(map);
		if map ~= mapId then
			if #temp > 0 then
				temp = temp .. ",";
			end
			temp = temp .. map;
		end
	end

	setInstanceVariable("available_maps", temp);
end

function clearManagedMap(mapId)
	local temp = getInstanceVariable("available_maps");
	if temp == nil then
		temp = "";
	end
	if #temp > 0 then
		temp = temp .. ",";
	end
	temp = temp .. mapId;

	showMapTimer(mapId, 0);
	setInstanceVariable("available_maps", temp);
end

function startManagedMapInstance(timerId, mapId, timeInSeconds)
	showMapTimer(mapId, timeInSeconds);
	startInstanceTimer("done_" .. timerId, timeInSeconds);
	takeManagedMap(mapId);
end

function getIdFromManagedMapTimer(name)
	local id = nil;
	local idx = name:find("done_");
	if idx ~= nil then
		id = tonumber(name:sub(6));
	end
	return id;
end

function cleanManagedMapInstance(timerId, mapId, stopTimer)
	if stopTimer == nil or stopTimer then
		stopInstanceTimer("done_" .. timerId);
	end
	clearManagedMap(mapId);
end