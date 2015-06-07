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
-- Jake - Subway Worker (Kerning City)

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

function generateChoice(choiceText, mapText, price, item, area)
	return makeChoiceData(choiceText, {mapText, price, item, area});
end

choices = {};

if getLevel() >= 20 then
	append(choices, generateChoice("Construction Site B1", "construction site B1", 500, 4031036, "Area 1"));
end
if getLevel() >= 30 then
	append(choices, generateChoice("Construction Site B2", "construction site B2", 1200, 4031037, "Area 2"));
end
if getLevel() >= 40 then
	append(choices, generateChoice("Construction Site B3", "construction site B3", 2000, 4031038, "Area 3"));
end

if getLevel() < 20 then
	addText("You must purchase the ticket to enter. ");
	addText("Once you have made the purchase, you can enter through The Ticket Gate on the right. ");
	addText("What would you like to buy?");
	sendNext();
else
	addText("You must purchase the ticket to enter. ");
	addText("Once you have made the purchase, you can enter through The Ticket Gate on the right. ");
	addText("What would you like to buy?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	mapText, price, item, area = data[1], data[2], data[3], data[4];

	addText("Will you purchase the ticket to " .. blue(mapText) .. "? ");
	addText("It'll cost you " .. price .. " mesos. ");
	addText("Before making the purchase, please make sure you have an empty slot on your etc. inventory.");
	answer = askYesNo();

	if answer == answer_no then
		addText("You can enter the premise once you have bought the ticket. ");
		addText("I heard there are strange devices in there everywhere but in the end, rare precious items await you. ");
		addText("So let me know if you ever decide to change your mind.");
		sendOk();
	else
		if item == nil or getMesos() < price or not giveItem(item, 1) then
			addText("Are you lacking mesos? ");
			addText("Check and see if you have an empty slot on your etc. inventory or not.");
		else
			giveMesos(-price);

			addText("You can insert the ticket in the The Ticket Gate. ");
			addText("I heard " .. area .. " has some precious items available but with so many traps all over the place most come back out early. ");
			addText("Please be safe.");
		end
		sendOk();
	end
end