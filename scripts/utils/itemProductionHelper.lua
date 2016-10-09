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
-- Functions to aid in the implementation of synthesizer NPCs

dofile("scripts/utils/npcHelper.lua");

-- displayResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by the constant item_mesos
-- Returns nothing, just displays the array
function displayResources(items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #reqs - 1, 2 do
		local item = reqs[index];
		local amt = reqs[index + 1];
		if item == item_mesos then
			addText(questMesosIcon(amt * multiplier) .. "\r\n");
		else
			addText(questItemIcon(item, amt * multiplier) .. "\r\n");
		end
	end
end

-- giveResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by the constant item_mesos
-- Returns nothing, just adds the items/mesos
function giveResources(items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #items - 1, 2 do
		if items[index] == item_mesos then
			giveMesos(multiplier * items[index + 1]);
		else
			giveItem(items[index], multiplier * items[index + 1]);
		end
	end
end

-- hasResources takes an array of requirements in the form of {itemid, amount, itemid, amount} and a multiplier
-- Returns true if the player has the resources or false if not
function hasResources(requisites, multiplier)
	local has = true;
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #requisites - 1, 2 do
		if requisites[index] == item_mesos then
			if getMesos() < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		else
			if getItemAmount(requisites[index]) < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		end
	end
	return has;
end

-- takeResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Returns nothing, just takes the items/mesos
function takeResources(requisites, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #requisites - 1, 2 do
		if requisites[index] == item_mesos then
			giveMesos(-1 * multiplier * requisites[index + 1]);
		else
			giveItem(requisites[index], -1 * multiplier * requisites[index + 1]);
		end
	end
end