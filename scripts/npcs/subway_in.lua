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
-- The Ticket Gate, collects tickets for Kerning Jump Quests and for NLC Subway

dofile("scripts/utils/boatHelper.lua");
dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

jumpTicketB1 = getItemAmount(4031036);
jumpTicketB2 = getItemAmount(4031037);
jumpTicketB3 = getItemAmount(4031038);
nlcTicketBasic = getItemAmount(4031710);
nlcTicketRegular = getItemAmount(4031711);

addText("Here's the ticket reader. ");

if jumpTicketB1 > 0 or jumpTicketB2 > 0 or jumpTicketB3 > 0 or nlcTicketBasic > 0 or nlcTicketRegular > 0 then
	choices = {};
	if jumpTicketB1 > 0 then
		append(choices, makeChoiceHandler("Construction site B1", function()
			giveItem(4031036, -1);
			setMap(103000900);
		end));
	end
	if jumpTicketB2 > 0 then
		append(choices, makeChoiceHandler("Construction site B2", function()
			giveItem(4031037, -1);
			setMap(103000903);
		end));
	end
	if jumpTicketB3 > 0 then
		append(choices, makeChoiceHandler("Construction site B3", function()
			giveItem(4031038, -1);
			setMap(103000906);
		end));
	end

	nlcChoice = nil;
	if nlcTicketBasic > 0 then
		nlcChoice = "New Leaf city (Basic)";
	elseif nlcTicketRegular > 0 then
		nlcChoice = "New Leaf city (Normal)";
	end

	if nlcChoice ~= nil then
		append(choices, makeChoiceHandler(nlcChoice, function()
			if setInstance("kerningToNlcBoarding") then
				minutesRemaining = getInstanceMinutes();

				if minutesRemaining == 0 then
					addText("We will begin boarding 5 minutes before the takeoff. ");
					addText("Please be patient and wait for a few minutes. ");
					addText("Be aware that the subway will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
					sendNext();
				elseif minutesRemaining == 1 then
					addText("This subway is getting ready for takeoff. ");
					addText("I'm sorry, but you'll have to get on the next ride. ");
					addText("The ride schedule is available through the usher at the ticketing booth.");
					sendNext();
				else
					addText("It looks like there's plenty of room for this ride. ");
					addText("Please have your ticket ready so I can let you in. ");
					addText("The ride will be long, but you'll get to your destination just fine. ");
					addText("What do you think? ");
					addText("Do you want to get on this ride?");
					answer = askYesNo();

					if answer == answer_yes then
						item = nil;
						if nlcTicketBasic > 0 then item = 4031710;
						else item = 4031711;
						end
						giveItem(item, -1);
						setMap(600010004);
					elseif answer == answer_no then
						addText("You must have some business to take care of here, right?");
						sendNext();
					end
				end
				revertInstance();
			else
				addText("We will begin boarding 5 minutes before the takeoff. ");
				addText("Please be patient and wait for a few minutes. ");
				addText("Be aware that the subway will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
				sendNext();
			end
		end));
	end

	addText("You will be brought in immediately. ");
	addText("Which ticket would you like to use?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	addText("You are not allowed in without the ticket.");
	sendOk();
end