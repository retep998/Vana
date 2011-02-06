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
-- The Ticket Gate, collects tickets for Kerning Jump Quests and for NLC Subway

dofile("scripts/lua_functions/miscFunctions.lua");

a = getItemAmount(4031036);
b = getItemAmount(4031037);
c = getItemAmount(4031038);
d = getItemAmount(4031711);
e = getItemAmount(4031710);

addText("Here's the ticket reader. ");

if a > 0 or b > 0 or c > 0 or d > 0 or e > 0 then
	addText("You will be brought in immediately. Which ticket would you like to use?\r\n");
	if a > 0 then
		addText("#b#L0#Construction site B1#l#k\r\n");
	end
	if b > 0 then
		addText("#b#L1#Construction site B2#l#k\r\n");
	end
	if c > 0 then
		addText("#b#L2#Construction site B3#l#k\r\n");
	end
	if d > 0 then
		addText("#b#L3#New Leaf city (Normal)#l#k\r\n");
	elseif e > 0 then
		addText("#b#L3#New Leaf city (Basic)#l#k\r\n");
	end

	choice = askChoice();
	map = 0;
	item = 0;
	if choice == 0 then
		map = 103000900;
		item = 4031036;
	elseif choice == 1 then
		map = 103000903;
		item = 4031037;
	elseif choice == 2 then
		map = 103000906;
		item = 4031038;
	elseif choice == 3 then
		if setInstance("kerningToNlcBoarding") then
			iTime = getInstanceMinutes();

			if iTime <= 1 then
	      			addText("This subway is getting ready for takeoff. I'm sorry, but you'll have to get on the next ride. The ride schedule is available through the usher at the ticketing booth.");
				sendNext();
			elseif iTime <= 5 then
				addText("It looks like there's plenty of room for this ride. Please have your ticket ready so I can let you in. The ride will be long, but you'll get to your destination just fine. What do you think? Do you want to get on this ride?");
				yes = askYesNo();

				if yes == 1 then		
					map = 600010004;
					item = 4031711;
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

	if map ~= 0 then
		giveItem(item, -1);
		setMap(map);
	end
else
	addText("You are not allowed in without the ticket.");
	sendOK();
end