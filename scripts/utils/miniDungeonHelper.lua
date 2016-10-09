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
-- A subset of Lua functions that helps with common minidungeon patterns

dofile("scripts/utils/mapManagerHelper.lua");
dofile("scripts/utils/partyQuestHelper.lua");
dofile("scripts/utils/tableHelper.lua");

local minidungeon_return_to_town = false;

function initMiniDungeonMaps(baseMapId, mapCount, entryMapId, townMapId)
	local maps = {};
	for i = 1, mapCount do
		append(maps, baseMapId + (i - 1));
	end
	initManagedMaps(maps);
	setInstanceVariable("entry_map", entryMapId);
	setInstanceVariable("town_map", townMapId);
end

function miniDungeonTimerEnd(name, fromTimer)
	if fromTimer then
		local mapId = getIdFromManagedMapTimer(name);
		if mapId then
			if minidungeon_return_to_town then
				moveAllMapMembers(mapId, getInstanceVariable("town_map", type_int));
			else
				moveAllMapMembers(mapId, getInstanceVariable("entry_map", type_int), "MD00");
			end
		end
	end
end

function miniDungeonPlayerDisconnect(playerId, isPartyLeader)
	if setPlayer(playerId) then
		local mapId = getMap();
		if getNumPlayers(mapId) == 0 then
			cleanManagedMapInstance(mapId, mapId);
		end
		revertPlayer();
	end
end

function miniDungeonPartyDisband(partyId)
	-- Intentionally left blank
	-- If you wanted to change minidungeons to spit out parties when they're disbanded, code would need to be added here
end

function miniDunegonPartyRemoveMember(partyId, playerId)
	-- Intentionally left blank
	-- If you wanted to change minidungeons to spit out players or parties when a player was kicked or left, code would need to be added here
end

function miniDungeonChangeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		removeInstancePlayer(playerId);
		if getNumPlayers(oldMap) == 1 then
			cleanManagedMapInstance(oldMap, oldMap);
		end
	elseif not isInstanceMap(oldMap) then
		addInstancePlayer(playerId);
	end
end

function miniDungeonPortal(instanceName)
	if setInstance(instanceName) then
		local entryMapId = getInstanceVariable("entry_map", type_int);

		if getMap() == entryMapId then
			local mapId = queryManagedMap(instanceName);
			if mapId == nil then
				-- TODO FIXME text
				showMessage("All of the Mini-Dungeons are in use right now, please try again later.", msg_red);
			else
				local dungeonTime = 2 * 60 * 60;
				startManagedMapInstance(mapId, mapId, dungeonTime);

				playPortalSe();
				if isPartyLeader() then
					moveLocalPartyMembersToMap(mapId);
				else
					setMap(mapId);
				end
			end
		else
			playPortalSe();
			if isPartyLeader() then
				moveLocalPartyMembersToMap(entryMapId, "MD00");
			else
				setMap(entryMapId, "MD00");
			end
		end
		revertInstance();
	else
		showMessage("The Mini Dungeon is not currently available.", msg_red);
	end
end