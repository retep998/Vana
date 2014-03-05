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
-- Alcaster

dofile("scripts/lua_functions/npcHelper.lua");

if isQuestCompleted(3035) then
	function generateChoice(itemId, price, effect)
		return makeChoiceData(" " .. itemRef(itemId) .. "(Price : " .. price .. " mesos)", {itemId, price, effect});
	end

	choices = {
		generateChoice(2050003, 300, " that cures the state of being sealed and cursed"),
		generateChoice(2050004, 400, " that cures all"),
		generateChoice(4006000, 5000, ", possessing magical power, that is used for high-quality skills"),
		generateChoice(4006001, 5000, ", possessing the power of summoning that is used for high-quality skills"),
	};

	addText("Thanks to you " .. blue(itemRef(4031056)) .. " is safely sealed. ");
	addText("Of course, also as a result, I used up about half of the power I have accumulated over the last 800 years or so...but now I can die in peace. ");
	addText("Oh, by the way... are you looking for rare items by any chance? ");
	addText("As a sign of appreciation for your hard work, I'll sell some items I have to you, and ONLY you. ");
	addText("Pick out the one you want!\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	itemId, itemPrice, itemDesc = data[1], data[2], data[3];

	addText("Is " .. blue(itemRef(itemId)) .. " really the item that you need? ");
	addText("It's the item" .. itemDesc .. ". ");
	addText("It may not be the easiest item to acquire, but I'll give you a good deal on it. ");
	addText("It'll cost you " .. blue(itemPrice .. " mesos") .. " per item. ");
	addText("How many would you like to purchase?");
	quantity = askNumber(0, 1, 100);

	local totalCost = itemPrice * quantity;
	addText("Are you sure you want to buy " .. red(quantity .. " " .. itemRef(itemId)) .. "? ");
	addText("It'll cost you " .. itemPrice .. " mesos per " .. itemRef(itemId) .. ", which will cost you " .. red(totalCost) .. " mesos total.");
	answer = askYesNo();

	if answer == answer_yes then
		if not hasOpenSlotsFor(itemId, quantity) or getMesos() < totalCost then
			addText("Are you sure you have enough mesos? ");
			addText("Please check and see if your etc. or use inventory is full, or if you have at least " .. red(totalCost) .. " mesos.");
			sendNext();
		else
			giveMesos(-totalCost);
			giveItem(itemId, quantity);

			addText("Thank you. ");
			addText("If you ever find yourself needing items down the road, make sure to drop by here. ");
			addText("I may have gotten old over the years, but I can still make magic items with ease.");
			sendNext();
		end
	end
else
	addText("If you decide to help me out, then in return, I'll make the item available for sale.");
	sendNext();
end