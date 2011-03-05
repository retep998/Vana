--[[
Copyright (C) 2008-2011 Vana Development Team

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

function getRandomFootholds(desiredamount, tablefootholds)
	i = 0;
	correctfootholds = {};
	while i < desiredamount do
		n = getRandomNumber(#tablefootholds);
		local add = true;
		if correctfootholds ~= nil then
			for v = 1, #correctfootholds do
				if correctfootholds[v] == n then
					add = false;
					break;
				end
			end
		end
		if add then
			i = i + 1;
			correctfootholds[i] = n;
		end
	end
	returnfootholds = {};
	for m = 1, desiredamount do
		returnfootholds[m] = tablefootholds[correctfootholds[m]];
	end
	return returnfootholds;
end

function saveFootholds(tablefootholds)
	setInstanceVariable("FootholdGroups", #tablefootholds);
	for i = 1, #tablefootholds do
		group = tablefootholds[i];
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

function giveAllPartyMembersEXP(exp, mapid)
	members = getAllPartyPlayerIDs();
	for i = 1, #members do
		member = members[i];
		if setPlayer(member) then
			if mapid == nil then
				giveEXP(exp);
			else
				if getMap() == mapid then
					giveEXP(exp);
				end
			end
			revertPlayer();
		end
	end
end

function moveAllMapMembers(mapid, portalstring)
	players = getAllMapPlayerIDs();
	for i = 1, #players do
		player = players[i];
		if setPlayer(player) then
			if portalstring == nil then
				setMap(mapid);
			else
				setMap(mapid, portalstring);
			end
			revertPlayer();
		end
	end
end