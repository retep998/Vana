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
-- Hotel Receptionist, Sleepywood sauna entry NPC

dofile("scripts/utils/npcHelper.lua");

addText("Welcome. ");
addText("We're the Sleepywood Hotel. ");
addText("Our hotel works hard to serve you the best at all times. ");
addText("If you are tired and worn out from hunting, how about a relaxing stay at our hotel?");
sendNext();

choices = {
	makeChoiceHandler("Regular sauna(499 mesos per use)", function()
		addText("You've chosen the regular sauna. ");
		addText("Your HP and MP will recover fast and you can even purchase some items there. ");
		return {499, 105040401};
	end),
	makeChoiceHandler("VIP sauna(999 mesos per use)", function()
		addText("You've chosen the VIP sauna. ");
		addText("Your HP and MP will recover even faster than that of the regular sauna and you can even find a special item in there. ");
		return {999, 105040402};
	end),
};

addText("We offer two kinds of rooms for service. ");
addText("Please choose the one of your liking.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

price, mapId = selectChoice(choices, choice);

addText("Are you sure you want to go in?");
answer = askYesNo();

if answer == answer_yes then
	if giveMesos(-price) then
		setMap(mapId);
	else
		addText("I'm sorry. ");
		addText("It looks like you don't have enough mesos. ");
		addText("It will cost you " .. price .. " mesos to stay at our hotel.");
		sendOk();
	end
else
	addText("We offer other kinds of services, too, so please think carefully and then make your decision.");
	sendOk();
end