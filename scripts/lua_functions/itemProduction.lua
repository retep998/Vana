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
-- Functions to aid in the implementation of synthesizer NPCs

MESOS = -1

-- displayResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by the constant MESOS
-- Returns nothing, just displays the array
function displayResources (items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #reqs - 1, 2 do
		local item = reqs[index];
		local amt = reqs[index + 1];
		if item == MESOS then -- Mesos are shown as item 4031138 (Sack of Money)
			addText("#v4031138# " .. amt .. " mesos");
		else
			if amt == 1 then
				addText("#v" .. item .. "# #t" .. item .. "#\r\n");
			else
				addText("#v" .. item .. "# " .. amt .. " #t" .. item .. "#s\r\n");
			end
		end
	end
end

-- giveResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by the constant MESOS
-- Returns nothing, just adds the items/mesos
function giveResources (items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1 ,#items - 1, 2 do
		if items[index] == MESOS then
			giveMesos((multiplier * items[index + 1]));
		else
			giveItem(items[index], (multiplier * items[index + 1]));
		end
	end
end

-- hasResources takes an array of requirements in the form of {itemid, amount, itemid, amount} and a multiplier
-- Returns true if the player has the resources or false if not
function hasResources (requisites, multiplier)
	local has = true;
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #requisites - 1, 2 do
		if requisites[index] == MESOS then
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
function takeResources (requisites, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1 ,#requisites - 1, 2 do
		if requisites[index] == MESOS then
			giveMesos((-1 * multiplier * requisites[index + 1]));
		else
			giveItem(requisites[index], (-1 * multiplier * requisites[index + 1]));
		end
	end
end
