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
-- Dolphin (Aquarium)

dofile("scripts/lua_functions/npcHelper.lua");

prices = nil;
if getJob() == 0 then
	prices = {100, 1000};
else
	prices = {1000, 10000};
end
maps = {230030200, 251000100};

addText("Oceans are all connected to each other. ");
addText("Places you can't reach by foot can be easily reached oversea. ");
addText("How about taking " .. blue("Dolphin Taxi") .. " with us today? ");
if getJob() == 0 then
	addText("We have special tickets with a 90% discount for Beginners! ");
end
addText("\r\n");

if getItemAmount(4031242) > 0 then
	addText(choiceRef("Go to " .. blue(mapRef(230030200)) .. " using " .. blue(itemRef(4031242)), 0));
else
	addText(choiceRef("Go to " .. blue(mapRef(230030200)) .. " after paying " .. prices[1] .. "mesos.", 0));
end

addText("\r\n" .. choiceRef("Go to " .. blue(mapRef(251000000)) .. " after paying " .. prices[2] .. "mesos.", 1) .. "\r\n");
where = askChoice() + 1;

if (where == 1 and getItemAmount(4031242) > 0 and giveItem(4031242, -1)) or giveMesos(-prices[where]) then -- Go to Sharp Unknown/Herb Town
	map = maps[where];
	portal = nil;
	if map == 230030200 then
		portal = "st00";
	end
	setMap(map, portal);
else
	addText("I don't think you have enough money...");
	sendNext();
end
