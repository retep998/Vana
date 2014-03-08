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
-- NLC Ticket Gate, collects tickets for NLC Subway

dofile("scripts/lua_functions/boatHelper.lua");
dofile("scripts/lua_functions/npcHelper.lua");

basicTicket = getItemAmount(4031712);
regularTicket = getItemAmount(4031713);

addText("Here's the ticket reader. ");

if regularTicket > 0 or basicTicket > 0 then
	addText("You will be brought in immediately. ");
	addText("Which ticket would you like to use?\r\n");

	if basicTicket > 0 then addText(blue(choiceRef("New Leaf city (Basic)"))));
	elseif regularTicket > 0 then addText(blue(choiceRef("New Leaf city (Normal)")));
	end

	choice = askChoice();
	if choice == 0 then
		if setInstance("nlcToKerningBoarding") then
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
				addText("What do you think? Do you want to get on this ride?");
				answer = askYesNo();

				if answer == answer_yes then
					item = nil;
					if basicTicket > 0 then item = 4031712;
					else item = 4031713;

					giveItem(item, -1);
					setMap(600010002);
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
	end
else
	addText("You are not allowed in without the ticket.");
	sendOk();
end