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
-- Spinel (Tour Guide, teleports to/from Zipangu)

dofile("scripts/lua_functions/npcHelper.lua");

if getMap() == 800000000 then
	wtOrigin = getPlayerVariable("wt_origin", type_int);
	if wtOrigin == nil then
		wtOrigin = 100000000;
	end

	choices = {
		makeChoiceHandler("Yes, I'm done with traveling. Can I go back to " .. mapRef(wtOrigin) .."? ", function()
			addText("Alright. ");
			addText("I'll now take you back to where you were before the visit to Japan. ");
			addText("If you ever feel like traveling again down the road, please let me know!");
			sendNext();

			setMap(wtOrigin);
			deletePlayerVariable("wt_origin");
		end),
		makeChoiceHandler("No, I'd like to continue exploring this place.", function()
			addText("OK. If you ever change your mind, please let me know.");
			sendOk();
		end),
	};

	addText("How's the traveling? ");
	addText("Are you enjoying it?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	addText("If you're tired of the monotonous daily life, how about getting out for a change? ");
	addText("There's nothing quite like soaking up a new culture, learning something new by the minute! ");
	addText("It's time for you to get out and travel. ");
	addText("We, at the Maple Travel Agency recommend you going on a " .. blue("World Tour") .. "! ");
	addText("Are you worried about the travel expense? ");

	if getJob() == 0 then
		addText("No need to worry! ");
		addText("The " .. blue("Maple Travel Agency") .. " offers first class travel accommodation for the low price of " .. blue("300 mesos"));
	else
		addText("You shouldn't be! ");
		addText("We, the " .. blue("Maple Travel Agency") .. ", have carefully come up with a plan to let you travel for ONLY " .. blue("3,000 mesos!"));
	end
	sendNext();

	addText("We currently offer this place for your traveling pleasure: " .. blue("Mushroom Shrine of Japan") .. ". ");
	addText("I'll be there serving you as the travel guide. ");
	addText("Rest assured, the number of destinations will increase over time. ");
	addText("Now, would you like to head over to the Mushroom Shrine?\r\n");
	addText(blue(choiceRef(" Yes, take me to Mushroom Shrine (Japan)")));
	choice = askChoice();

	addText("Would you like to travel to " .. blue("Mushroom Shrine of Japan") .. "? ");
	addText("If you desire to feel the essence of Japan, there's nothing like visiting the Shrine, a Japanese cultural melting pot. ");
	addText("Mushroom Shrine is a mythical place that serves the incomparable Mushroom God from ancient times.");
	sendNext();

	addText("Check out the female shaman serving the Mushroom God, and I strongly recommend trying Takoyaki, Yakisoba, and other delicious food sold in the streets of Japan. ");
	addText("Now, let's head over to " .. blue("Mushroom Shrine") .. ", a mythical place if there ever was one.");
	sendBackNext();

	if getJob() == 0 then
		price = 300;
	else
		price = 3000;
	end
	if giveMesos(-price) then
		setPlayerVariable("wt_origin", getMap());
		setMap(800000000);
	else
		addText("Please check and see if you have enough mesos to go.");
		sendBackOk();
	end
end