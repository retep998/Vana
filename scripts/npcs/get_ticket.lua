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
-- Boat ticket takers

dofile("scripts/utils/boatHelper.lua");
dofile("scripts/utils/npcHelper.lua");

function getTicketType(basic, regular)
	if getItemAmount(basic) > 0 then return basic; end
	return regular;
end

function getOrbisTicket()
	return getTicketType(4031044, 4031045);
end

m = getMap();
destinationMap = nil;
item = nil;
instanceName = nil;
-- TODO FIXME implement some kind of configuration or feature testing for this instead of having it be a constant
freeFlights = true;

if m == 101000300 then
	destinationMap = 101000301;
	item = getOrbisTicket();
	instanceName = "elliniaToOrbisBoarding";
elseif m == 240000110 then
	destinationMap = 240000111;
	item = getOrbisTicket();
	instanceName = "leafreToOrbisBoarding";
elseif m == 220000110 then
	destinationMap = 220000111;
	item = getOrbisTicket();
	instanceName = "ludiToOrbisBoarding";
elseif m == 260000100 then
	destinationMap = 260000110;
	item = getOrbisTicket();
	instanceName = "ariantToOrbisBoarding";
elseif m == 200000111 then
	destinationMap = 200000112;
	item = getTicketType(4031046, 4031047);
	instanceName = "orbisToElliniaBoarding";
elseif m == 200000131 then
	destinationMap = 200000132;
	item = getTicketType(4031330, 4031331);
	instanceName = "orbisToLeafreBoarding";
elseif m == 200000121 then
	destinationMap = 200000122;
	item = getTicketType(4031073, 4031074);
	instanceName = "orbisToLudiBoarding";
elseif m == 200000151 then
	destinationMap = 200000152;
	item = getTicketType(4031575, 4031576);
	instanceName = "orbisToAriantBoarding";
else
	consoleOutput("Unsupported get_ticket map: " .. m);
	return;
end

if setInstance(instanceName) then
	timeLeft = getInstanceMinutes();
	instanceTime = getInstanceVariable("boat_time", type_int);
	boardTime = instanceTime - 5;

	if timeLeft <= 1 then
		addText("This ship is getting ready for takeoff. ");
		addText("I'm sorry, but you'll have to get on the next ride. ");
		addText("The ride schedule is available through the usher at the ticketing booth.");
		sendNext();
	elseif timeLeft <= boardTime then
		if freeFlights then
			addText("This will not be a short flight, so if you need to take care of some things, I suggest you do that first before getting on board. ");
			addText("Do you still wish to board the ship?");
		else
			addText("It looks like there's plenty of room for the ride. ");
			addText("Please have your ticket ready so I can let you in. ");
			addText("The ride will be long, but you'll get to your destination just fine. ");
			addText("What do you think? ");
			addText("Do you want to get on the ride?");
		end
		answer = askYesNo();

		if answer == answer_yes then
			-- Ensure that the boarding feasibility is still valid after the answer, player could easily have held the dialog open
			timeLeft = getInstanceMinutes();
			if timeLeft <= 1 then
				addText("This ship is getting ready for takeoff. ");
				addText("I'm sorry, but you'll have to get on the next ride. ");
				addText("The ride schedule is available through the usher at the ticketing booth.");
				sendNext();
			elseif timeLeft <= boardTime then
				if not freeFlights and not giveItem(item, -1) then
					addText("Oh no... ");
					addText("I don't think you have the ticket with you. ");
					addText("I can't let you in without it. ");
					addText("Please buy the ticket at the ticketing booth.");
					sendNext();
				else
					setMap(destinationMap);
				end
			else
				addText("We will begin boarding " .. boardTime .. " minutes before the takeoff. ");
				addText("Please be patient and wait for a few minutes. ");
				if freeFlights then
					addText("Be aware that the ship will take off right on time, and we stop boarding 1 minute before that, so please make sure to be here on time.");
				else
					addText("Be aware that the ship will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
				end
				sendNext();
			end
		else
			addText("You must have some business to take care of here, right?");
			sendNext();
		end
	else
		addText("We will begin boarding " .. boardTime .. " minutes before the takeoff. ");
		addText("Please be patient and wait for a few minutes. ");
		if freeFlights then
			addText("Be aware that the ship will take off right on time, and we stop boarding 1 minute before that, so please make sure to be here on time.");
		else
			addText("Be aware that the ship will take off right on time, and we stop receiving tickets 1 minute before that, so please make sure to be here on time.");
		end
		sendNext();
	end

	revertInstance();
end