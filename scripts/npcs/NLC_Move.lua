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
-- NLC Ticket Gate, collects tickets for NLC Subway

dofile("scripts/lua_functions/miscFunctions.lua");

a = getItemAmount(4031713);
b = getItemAmount(4031712);

addText("Here's the ticket reader. ");

if a > 0 or b > 0 then
	 addText("You will be brought in immediately. Which ticket would you like to use?\r\n");
	if a > 0 then
		addText("#b#L0#New Leaf city (Normal)#l#k\r\n");
	elseif b > 0 then
		addText("#b#L0#New Leaf city (Basic)#l#k\r\n");
	end

	choice = askChoice();
	if choice == 0 then
		if setInstance("nlcToKerningBoarding") then
			iTime = getInstanceMinutes();

			if iTime <= 1 then
	      			addText("This subway is getting ready for takeoff. I'm sorry, but you'll have to get on the next ride. The ride schedule is available through the usher at the ticketing booth.");
				sendNext();
			elseif iTime <= 5 then
				addText("It looks like there's plenty of room for this ride. Please have your ticket ready so I can let you in. The ride will be long, but you'll get to your destination just fine. What do you think? Do you want to get on this ride?");
				yes = askYesNo();

				if yes == 1 then		
					giveItem(4031713, -1);
					setMap(600010002);
				elseif yes == 0 then
					addText("You must have some business to take care of here, right?");
					sendNext();
				end
			else
				addText("We will begin boarding 5 minutes before the takeoff. Please be patient and wait for a few minutes. Be aware that the subway will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
				sendNext();
			end
			revertInstance();
		else
			addText("We will begin boarding 5 minutes before the takeoff. Please be patient and wait for a few minutes. Be aware that the subway will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
			sendNext();
		end
	end
else
	addText("You are not allowed in without the ticket.");
	sendOK();
end