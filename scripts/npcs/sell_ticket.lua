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
-- Agatha, Syras, Mue, Joel, Mel, travel guides

dofile("scripts/lua_functions/miscFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

if getLevel() < 10 then
	addText("Your level seems to be too low for this. ");
	addText("We do not allow anyone below Level 10 to get on this ride, for the sake of safety.");
	sendNext();

	return;
end

function getBoatTime(instanceName)
	if setInstance(instanceName) then
		local temp = getInstanceVariable("boat_time", type_int);
		revertInstance();
		return temp;
	end
	return nil;
end

function getTicket(basic, regular)
	if getLevel() < 30 then
		return basic;
	end
	return regular;
end

function getOrbisTicket()
	return getTicket(4031044, 4031045);
end

function getPrice(basic, regular)
	if getLevel() < 30 then
		return basic;
	end
	return regular;
end

function platformInfoToOrbis(place, boatTime, boatType, platformNpc, platformNpcThroughPortal)
	addText("Hi there! ");
	addText("I'm " .. npcRef(getNpcId()) .. ", and I work in this station. ");
	addText("Are you thinking of leaving " .. place .. " for other places? ");
	addText("This station is where you'll find the " .. boatType .. " that heads to " .. blue("Orbis Station") .. " of Ossyria leaving " .. blue("at the top of the hour, and every " .. boatTime .. " minutes afterwards") .. ".");
	sendNext();

	if platformNpcThroughPortal then
		addText("If you are thinking or going to Orbis, please use the portal on the right and head to the station, then talk to " .. blue(npcRef(platformNpc)) .. ".");
	else
		addText("If you are thinking or going to Orbis, please go talk to " .. blue(npcRef(platformNpc)) .. " on the right.");
	end
	sendBackNext();

	addText("Well, the truth is, we charged for these flights until very recently, but the alchemists of Magatia made a crucial discovery on the fuel that dramatically cuts down the amount of Mana used for the flight, so these flights are now free. ");
	addText("Don't worry, we still get paid. ");
	addText("Now we just get paid through the government.");
	sendBackOk();
end

function buyTicketToOrbis(itemId, boatTime, price)
	addText("Hello, I'm in charge of selling tickets for the ship ride to Orbis Station of Ossyria. ");
	addText("The ride to Orbis takes off every " .. boatTime .. " minutes, beginning on the hour, and it'll cost you " .. blue(price .. " mesos") .. ". ");
	addText("Are you sure you want to purchase " .. blue(itemRef(item)) .. "?");
	answer = askYesNo();

	if answer == answer_no then
		addText("You must have some business to take care of here, right?");
		sendNext();
	else
		if not hasOpenSlotsFor(itemId) or not giveMesos(-price) then
			addText("Are you sure you have " .. blue(price .. " mesos") .. "? ");
			addText("If so, then I urge you to check your etc. inventory, and see if it's full or not.");
			sendOk();
		else
			giveItem(itemId, 1);
		end
	end
end

freeFlights = true;
m = getMap();

if m == 101000300 then
	item = getOrbisTicket();
	price = getPrice(1000, 5000);
	boatTime = getBoatTime("elliniaToOrbisBoarding");

	if freeFlights then
		platformInfoToOrbis("Victoria Island", boatTime, "ship", 1032008, false);
	else		
		buyTicketToOrbis(item, boatTime, price);
	end
elseif m == 240000100 then
	item = getOrbisTicket();
	price = getPrice(2000, 30000);
	boatTime = getBoatTime("leafreToOrbisBoarding");

	if freeFlights then
		platformInfoToOrbis("Leafre", boatTime, "ship", 2082001, true);
	else
		buyTicketToOrbis(item, boatTime, price);
	end
elseif m == 260000100 then
	item = getOrbisTicket();
	price = getPrice(2000, 6000);
	boatTime = getBoatTime("ariantToOrbisBoarding");

	if freeFlights then
		platformInfoToOrbis("Ariant", boatTime, "geenie", 2102000, false);
	else
		buyTicketToOrbis(item, boatTime, price);
	end
elseif m == 220000100 then
	item = getOrbisTicket();
	price = getPrice(2000, 6000);
	boatTime = getBoatTime("ludiToOrbisBoarding");

	if freeFlights then
		platformInfoToOrbis("Ludibrium", boatTime, "ship", 2041000, true);
	else
		buyTicketToOrbis(item, boatTime, price);
	end
elseif m == 200000100 then
	if freeFlights then
		choices = {
			makeChoiceHandler("Ellinia of Victoria Island", function()
				addText("Do you want to go to Ellinia of Victoria Island? ");
				addText("It's a beautiful, sky-scraping forest that mainly consists of trees that have been around for generations. ");
				addText("The ship that heads to Ellinia " .. blue("leaves at the top of the hour, and every " .. getBoatTime("orbisToElliniaBoarding") .. " minutes afterwards") .. ".");
				sendNext();

				return "Ellinia of Victoria Island";
			end),
			makeChoiceHandler("Ludibrium", function()
				addText("Are you heading towards Ludibrium in Lake Ludus? ");
				addText("It's a fun little town that mainly consists of toys. ");
				addText("The ship that heads to Ludibrium " .. blue("leaves at the top of the hour, and every " .. getBoatTime("orbisToLudiBoarding") .. " minutes afterwards") .. ".");
				sendNext();

				return "Ludibrium";
			end),
			makeChoiceHandler("Leafre", function()
				addText("Are you heading towards Leafre of Minar Forest? ");
				addText("It's a cozy little town where the Hafrings reside. ");
				addText("The ship that heads to Leafre " .. blue("leaves at the top of the hour, and every " .. getBoatTime("orbisToLeafreBoarding") .. " minutes afterwards") .. ".");
				sendNext();

				return "Leafre";
			end),
			makeChoiceHandler("Mu Lung", function()
				addText("Are you heading towards Mu Lung in the Mu Lung Temple? ");
				addText("There's a " .. blue("Crane that runs a cab service for 1 that's always available") .. ", so you'll get there as soon as you wish.\r\n\r\n");
				addText("Unlike the trips that are for free, this trip requires a set fee. ");
				addText("This trip will cost you " .. blue("1,500 mesos") .. ", so please have the fee ready before riding the Crane. ");

				return "Mu Lung";
			end),
			makeChoiceHandler("Ariant", function()
				addText("Are you heading towards Ariant in Nihal Desert? ");
				addText("It's a town full of alchemists that live the life with vigor much like the scorching desert heat. ");
				addText("The ship that heads to Ariant " .. blue("leaves at the top of the hour, and every " .. getBoatTime("orbisToAriantBoarding") .. " minutes afterwards") .. ".");
				sendNext();

				return "Ariant";
			end),
			makeChoiceHandler("Ereve", function()
				addText("Are you heading toward Ereve? ");
				addText("It's a beautiful island blessed by Shinsoo and Empress Cygnus and her knights are living in. ");
				addText(blue("The boat is for 1 person and it's always readily available") .. " so you can travel to Ereve fast.");
				sendNext();

				return "Ereve";
			end),
		};

		addText("Hello there! ");
		addText("I am here to lead you to the trip that will take you to the destination of your choice. ");
		addText("Where woulud you like to go?\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		text = selectChoice(choices, choice);
		addText("If you wish to hop on board the ship that'll fly to " .. text .. ", talk to " .. blue("Isa the Station Guide") .. " on the right. ");
		addText("She'll take you to the station where the trip to " .. text .. " awaits.");
		sendOk();
	else
		function makeTravelChoice(label, text, instanceName, price, item)
			return makeChoiceData(label, {text, getBoatTime(instanceName), price, item});
		end

		choices = {
			makeTravelChoice("Ellinia of Victoria Island", "Ellinia of Victoria", "orbisToElliniaBoarding", getPrice(1000, 5000), getTicket(4031046, 4031047)),
			makeTravelChoice("Ludibrium", "Ludibrium", "orbisToLudiBoarding", getPrice(2000, 6000), getTicket(4031073, 4031074)),
			makeTravelChoice("Leafre", "Leafre of Minar Forest", "orbisToLeafreBoarding", getPrice(10000, 30000), getTicket(4031330, 4031331)),
			makeTravelChoice("Ariant", "Nihal Desert", "orbisToAriantBoarding", getPrice(2000, 6000), getTicket(4031575, 4031576)),
		};

		addText("Hello, I'm in charge of selling tickets for the ship ride for every destination. ");
		addText("Which ticket would you like to purchase?\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		data = selectChoice(choices, choice);
		text, boatTime, price, item = data[1], data[2], data[3], data[4];

		addText("The ride to " .. text .. " takes off every " .. boatTime .. " minutes, beginning on the hour, and it'll cost you " .. blue(price .. " mesos") .. ". ");
		addText("Are you sure you want to purchase " .. blue(itemRef(item)) .. "?");
		answer = askYesNo();

		if answer == answer_no then
			addText("You must have some business to take care of here, right?");
			sendNext();
		else
			if not hasOpenSlotsFor(itemId) or not giveMesos(-price) then
				addText("Are you sure you have " .. blue(price .. " mesos") .. "? ");
				addText("If so, then I urge you to check your etc. inventory, and see if it's full or not.");
				sendOk();
			else
				giveItem(itemId, 1);
			end
		end	
	end
end