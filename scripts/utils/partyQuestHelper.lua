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
-- A subset of Lua functions that helps common party quest tasks

function getRandomFootholds(desiredAmount, tableFootholds)
	local i = 0;
	local correctFootholds = {};
	while i < desiredAmount do
		local n = getRandomNumber(#tableFootholds);
		local add = true;
		if correctFootholds ~= nil then
			for v = 1, #correctFootholds do
				if correctFootholds[v] == n then
					add = false;
					break;
				end
			end
		end
		if add then
			i = i + 1;
			correctFootholds[i] = n;
		end
	end
	local returnFootholds = {};
	for m = 1, desiredAmount do
		returnFootholds[m] = tableFootholds[correctFootholds[m]];
	end
	return returnFootholds;
end

function partyQuestClear()
	showMapEvent("quest/party/clear");
	playFieldSound("Party1/Clear", getMap());
end

function partyQuestWrong()
	showMapEvent("quest/party/wrong_kor");
	playFieldSound("Party1/Failed", getMap());
end

function giveAllPartyMembersExp(expAmount, mapId)
	local members = getAllPartyPlayerIds();
	for i = 1, #members do
		local member = members[i];
		if setPlayer(member) then
			if mapId == nil then
				giveExp(expAmount);
			else
				if getMap() == mapId then
					giveExp(expAmount);
				end
			end
			revertPlayer();
		end
	end
end

function moveAllMapMembers(fromMapId, toMapId, portalString)
	local players = getAllMapPlayerIds(fromMapId);
	for i = 1, #players do
		local player = players[i];
		if setPlayer(player) then
			if portalString == nil then
				setMap(toMapId);
			else
				setMap(toMapId, portalString);
			end
			revertPlayer();
		end
	end
end

function addPartyMembersToInstance()
	local players = getAllPartyPlayerIds();
	local mapId = getMap();
	for i = 1, #players do
		local player = players[i];
		if setPlayer(player) then
			if getMap() == mapId then
				addInstancePlayer(getId());
			end
			revertPlayer();
		end
	end
end

function moveLocalPartyMembersToMap(destinationMapId, portalString)
	local players = getAllPartyPlayerIds();
	local currentMapId = getMap();
	for i = 1, #players do
		local player = players[i];
		if setPlayer(player) then
			if getMap() == currentMapId then
				if portalString == nil then
					setMap(destinationMapId);
				else
					setMap(destinationMapId, portalString);
				end
			end
			revertPlayer();
		end
	end
end