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
-- Shalon, tickets to Kerning + flight map

dofile("scripts/lua_functions/boatHelper.lua");
dofile("scripts/lua_functions/npcHelper.lua");

choices = {
	makeChoiceHandler("I would like to buy a plane ticket to Kerning City", function()
		addText("The ticket will cost you 20,000 mesos. ");
		addText("Will you purchase the ticket?");
		answer = askYesNo();

		if answer == answer_yes then
			if not hasOpenSlotsFor(4031732) or not giveMesos(-20000) then
				addText("I don't think you have enough meso or empty slot in your ETC inventory. ");
				addText("Please check and talk to me again.");
				sendNext();
			else
				giveItem(4031732, 1);
			end
		else
			addText("I am here for a long time. ");
			addText("Please talk to me again when you change your mind.");
			sendNext();
		end
	end),
	makeChoiceHandler("Let me go in to the departure point.", function()
		if setInstance("cbdToKerningBoarding") then
			timeLeft = getInstanceMinutes();
			if timeLeft <= 1 then
				addText("We are sorry but the gate is closed 1 minute before the departure.");
				sendNext();
			else
				addText("Would you like to go in now? ");
				addText("You will lose your ticket once you go in~ ");
				addText("Thank you for choosing Wizet Airline.");
				answer = askYesNo();

				if answer == answer_yes then
					timeLeft = getInstanceMinutes();
					if timeLeft <= 1 then
						addText("We are sorry but the gate is closed 1 minute before the departure.");
						sendNext();
					elseif not giveItem(4031732, -1) then
						addText("Please do purchase the ticket first. ");
						addText("Thank you~");
						sendNext();
					else
						setMap(540010001);
					end
				else
					addText("Please confirm the departure time you wish to leave. ");
					addText("Thank you.");
					sendNext();
				end
			end
			revertInstance();
		end
	end),	
};

addText("Hello there~ ");
addText("I am Shalon from Singapore Airport. ");
addText("I will assist you in getting back to Kerning City in no time. ");
addText("How can I help you?\r\n");
addText(blue(choiceList(choices)));
choice = askChoice();

selectChoice(choices, choice);