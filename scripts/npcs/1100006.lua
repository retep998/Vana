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
-- Kiru (on the boat between Ereve and Orbis)

dofile("scripts/lua_functions/npcHelper.lua");

isToEreve = (getMap() % 2) == 0;
if isToEreve then
	choices = {
		makeChoiceHandler("Yes, tell me.", function()
			addText("Ereve is a floating island that is protected by Shinsoo. ");
			addText("Yes, floating in the air! ");
			addText("It's stationary now but it used to travel throughout the world so that the Empress could watch over everything.");
			sendNext();

			addText("As the Black Mage gains strength Empress Cygnus thought it was a good idea to stop traveling so that she could concentrate on acquiring more strength herself. ");
			addText("That is how the Cygnus Knights were formed.");
			sendBackNext();

			addText("Having said that though, the Cygnus Knights don't actually stay on the island very long. ");
			addText("As their levels increase, they leave to train in other places as well as to keep tabs on the Black Mage's movements.");
			sendBackNext();

			addText("That is why there are usually only low-level knights on the island. ");
			addText("Oh, and our race, too, of course! ");
			addText("Do you know about our race? ");
			addText("We are a race called the Piyo. ");
			addText("You can consider us distant relatives of Shinsoo. ");
			addText("That is why we assist with the upkeep of the island.");
			sendBackNext();

			addText("In other words, we are not subordinates of the Empress. ");
			addText("We are only subordinates of Shinsoo. ");
			addText("We assist the Empress because Shinsoo is protecting her, but we don't follow her orders.");
			sendBackNext();

			addText(blue("(Kiru continued to talk for a long time...)"));
			sendBackNext();
		end),
		makeChoiceHandler("No, it's okay.", function()
			-- Intentionally blank
		end),
	};

	addText("Ah, such lovely winds. ");
	addText("This should be a perfect voyage as long as no stupid customer falls off for attempting some weird skill. ");
	addText("Of course, I'm talking about you. ");
	addText("Please refrain from using your skills. ");
	addText("If you're bored, do you want me to tell you about Ereve?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	choices = {
		makeChoiceHandler("Yes, I want to know.", function()
			addText("Orbis is a town at the top of a tower in the northernmost region of Ossyria. ");
			addText("It's way up in the sky. ");
			addText("In that respect, it's very similar to Ereve. ");
			addText("Although I guess it's a bit different since the entire island of Ereve floats...");
			sendNext();

			addText("Orbis is the continental center of transportation. ");
			addText("The Orbis Station is full of sky ferries that travel to different areas. ");
			addText("Although some of the vehicles don't necessarily look like sky ferries... ");
			addText("Regardless, you'll often have to pass through Orbis on your way to different places.");
			sendBackNext();

			addText("There are a lot of fairies that live in Orbis and they are a little different from the fairies in Ellinia, because they have feathered wings. ");
			addText("They are also more gentle and actually get along with humans. ");
			addText("Although I guess not all of them are like that...");
			sendBackNext();

			addText("(Kiru continued to talk for a long time...)");
			sendBackNext();
		end),
		makeChoiceHandler("No, it's okay...", function()
			addText("Well, okay. ");
			addText("Don't do anything weird because you're bored.");
			sendNext();
		end),
	};

	addText("Ah, good winds have graced us. ");
	addText("If this continues, we should be able to arrive in Orbis safely. ");
	addText("Wait, you over there. ");
	addText("Don't move or jump around while we are flying. ");
	addText("And don't use your skills. ");
	addText("It's very dangerous...huh? ");
	addText("Do you want to know about Orbis?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
end