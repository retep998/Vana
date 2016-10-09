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
-- Pason and Shuri (Tour Guides, teleport to Florina Beach)

dofile("scripts/utils/npcHelper.lua");

ticket = 4031134;
florina = 110000000;

choices = {
	makeChoiceHandler(" I'll pay 1500 mesos.", function()
		addText("So you want to pay " .. blue("1500 mesos") .. " and leave for " .. mapRef(florina) .. "? ");
		addText("Alright, then, but just be aware that you may be running into some monsters around there, too. ");
		addText("Okay, would you like to head over to " .. mapRef(florina) .. " right now?");
		answer = askYesNo();

		if answer == answer_yes then
			if giveMesos(-1500) then
				setPlayerVariable("florina_origin", getMap());
				setMap(florina);
			else
				addText("I think you're lacking mesos. ");
				addText("There are many ways to gather up some money, you know, like ... selling your armor ... defeating the monsters ... doing quests ... you know what I'm talking about.");
				sendNext();
			end
		else
			addText("You must have some business to take care of here. ");
			addText("You must be tired from all that traveling and hunting. ");
			addText("Go take some rest, and if you feel like changing your mind, then come talk to me.");
			sendNext();
		end
	end),
	makeChoiceHandler(" I have " .. itemRef(ticket) .. ".", function()
		addText("So you have " .. blue(itemRef(ticket)) .. "? ");
		addText("You can always head over to " .. mapRef(florina) .. " with that ");
		addText("Alright, then, but just be aware that you may be running into some monsters there, too. ");
		addText("Okay, would you like to head over to  " .. mapRef(florina) .. " right now?");
		answer = askYesNo();

		if answer == answer_yes then
			if getItemAmount(ticket) >= 1 then
				setPlayerVariable("florina_origin", getMap());
				setMap(florina);
			else
				addText("Hmmm, so where exactly is " .. blue(itemRef(ticket)) .. "?? ");
				addText("Are you sure you have them? ");
				addText("Please double-check.");
				sendNext();
			end
		else
			addText("You must have some business to take care of here. You must be tired from all that traveling and hunting. Go take some rest, and if you feel like changing your mind, then come talk to me.");
			sendNext();
		end
	end),
	makeChoiceHandler(" What is " .. itemRef(ticket) .. "?", function()
		addText("You must be curious about " .. blue(itemRef(ticket)) .. ". ");
		addText("Haha, that's very understandable. ");
		addText(itemRef(ticket) .. " is an item where as long as you have in possession, you may make your way to " .. mapRef(florina) .. " for free. ");
		addText("It's such a rare item that even we had to buy those, but unfortunately I lost mine a few weeks ago during my precious summer break.");
		sendNext();

		addText("I came back without it, and it just feels awful not having it. ");
		addText("Hopefully someone picked it up and put it somewhere safe. ");
		addText("Anyway this is my story and who knows, you may be able to pick it up and put it to good use. ");
		addText("If you have any questions, feel free to ask.");
		sendBackNext();
	end),
};

addText("Have you heard of the beach with a spectacular view of the ocean called " .. blue(mapRef(florina)) .. ", located near " .. mapRef(getMap()) .. "? ");
addText("I can take you there right now for either " .. blue("1500 mesos") .. ", or if you have " .. blue(itemRef(ticket)) .. " with you, in which case you'll be in for free.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);