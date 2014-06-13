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
dofile("scripts/lua_functions/tableHelper.lua");

function getPrice(beginner, regular)
	if getJob() == 0 then
		return beginner;
	end
	return regular;
end

choices = {};
hasQuestTicket = getItemAmount(4031242) > 0;
if hasQuestTicket then
	append(choices, makeChoiceData("Go to " .. blue(mapRef(230030200)) .. " using " .. blue(itemRef(4031242)), {230030200}));
else
	price = getPrice(100, 1000);
	append(choices, makeChoiceData("Go to " .. blue(mapRef(230030200)) .. " after paying " .. price .. "mesos.", {230030200, price}));
end

price = getPrice(1000, 10000);
append(choices, makeChoiceData("Go to " .. blue(mapRef(251000000)) .. " after paying " .. price .. "mesos.", {251000100, price}));

addText("Oceans are all connected to each other. ");
addText("Places you can't reach by foot can be easily reached oversea. ");
addText("How about taking " .. blue("Dolphin Taxi") .. " with us today? ");
if getJob() == 0 then
	addText("We have special tickets with a 90% discount for Beginners! ");
end
addText("\r\n");
addText(choiceRef(choices));
choice = askChoice();

data = selectChoice(choices, choice);
mapId, price = data[1], data[2];

portal = nil;
if mapId == 230030200 then
	portal = "st00";
end

if price == nil then
	if giveItem(4031242, -1) then
		setMap(mapId, portal);
	else
		addText("I don't think you have enough money...");
		sendNext();
	end
elseif giveMesos(-price) then
	setMap(mapId, portal);
else
	addText("I don't think you have enough money...");
	sendNext();
end