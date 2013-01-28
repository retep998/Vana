--[[
Copyright (C) 2008-2013 Vana Development Team

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
	i = 0;
	correctFootholds = {};
	while i < desiredAmount do
		n = getRandomNumber(#tableFootholds);
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
	returnFootholds = {};
	for m = 1, desiredAmount do
		returnFootholds[m] = tableFootholds[correctFootholds[m]];
	end
	return returnFootholds;
end

function saveFootholds(tableFootholds)
	setInstanceVariable("FootholdGroups", #tableFootholds);
	for i = 1, #tableFootholds do
		group = tableFootholds[i];
		setInstanceVariable("FootholdGroup" .. i .. "Count", #group);
		for k = 1, #group do
			setInstanceVariable("FootholdGroup" .. i .. "Element" .. k, group[k]);
		end
	end
end

function getSavedFootholds()
	footholds = {};
	group = {};
	n = getInstanceVariable("FootholdGroups", true);
	for i = 1, n do
		p = getInstanceVariable("FootholdGroup" .. i .. "Count", true);
		for k = 1, p do
			group[k] = tonumber(getInstanceVariable("FootholdGroup" .. i .. "Element" .. k));
		end
		footholds[i] = group;
		group = {};
	end
	return footholds;
end

function partyQuestClear()
	showMapEvent("quest/party/clear");
	playFieldSound("Party1/Clear", getMap());
end

function partyQuestWrong()
	showMapEvent("quest/party/wrong_kor");
	playFieldSound("Party1/Failed", getMap());
end

function giveAllPartyMembersExp(exp, mapId)
	members = getAllPartyPlayerIds();
	for i = 1, #members do
		member = members[i];
		if setPlayer(member) then
			if mapId == nil then
				giveExp(exp);
			else
				if getMap() == mapId then
					giveExp(exp);
				end
			end
			revertPlayer();
		end
	end
end

function moveAllMapMembers(mapId, portalString)
	players = getAllMapPlayerIds();
	for i = 1, #players do
		player = players[i];
		if setPlayer(player) then
			if portalString == nil then
				setMap(mapId);
			else
				setMap(mapId, portalString);
			end
			revertPlayer();
		end
	end
end

function addPartyMembersToInstance()
	players = getAllPartyPlayerIds();
	mapId = getMap();
	for i = 1, #players do
		player = players[i];
		if setPlayer(player) then
			if getMap() == mapId then
				addInstancePlayer(getId());
			end
			revertPlayer();
		end
	end
end

function moveLocalPartyMembersToMap(destinationMapId, portalString)
	players = getAllPartyPlayerIds();
	currentMapId = getMap();
	for i = 1, #players do
		player = players[i];
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