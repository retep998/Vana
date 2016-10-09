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
-- Jane in Lith (1002100)

dofile("scripts/utils/npcHelper.lua");

if isQuestCompleted(2013) then
	addText("It's you...! ");
	addText("Thanks to you, I was able to get much done. ");
	addText("Nowadays, I've been making a bunch of items. ");
	addText("If you need anything, just let me know.");
	sendNext();

	function generateChoice(itemId, price, effect)
		return makeChoiceData(itemRef(itemId) .. " (price : " .. price .. " mesos) ", {itemId, price, effect});
	end

	choices = {
		generateChoice(2000002, 310, "allows you to recover 300 HP"),
		generateChoice(2022003, 1060, "allows you to recover 1000 HP"),
		generateChoice(2022000, 1600, "allows you to recover 800 MP"),
		generateChoice(2001000, 3120, "allows you to recover 1000 HP and MP"),
	};

	addText("What would you like to buy? \r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	item, price, effect = data[1], data[2], data[3];

	addText("You want " .. blue(itemRef(item)) .. "? ");
	addText(itemRef(item) .. " " .. effect .. ". ");
	addText("How many would you like to purchase?");
	qty = askNumber(0, 1, 100);

	finalCost = qty * price;

	addText("Will you purchase " .. red(qty) .. " " .. blue(itemRef(item) .. "(s)") .. "? ");
	addText(blue(itemRef(item)) .. " costs " .. price .. " per piece, so the total comes out to be " .. finalCost .. " mesos.");
	answer = askYesNo();

	if answer == answer_yes then
		if hasOpenSlotsFor(item, qty) and getMesos() >= finalCost then
			giveItem(item, qty);
			giveMesos(-finalCost);
			addText("Thank you for coming! ");
			addText("Stuff here can always be made, so if you need something, please come again.");
			sendNext();
		else
			addText("Are you lacking mesos by any chance? ");
			addText("Please check to see if you have an empty slot available in the Etc window of your Item Inventory, and if you have at least " .. red(finalCost) .. " mesos with you.");
			sendNext();
		end
	else
		addText("I still have quite a few of the materials you got me before. ");
		addText("The items are all there, so take your time in choosing.");
		sendNext();
	end
else
	addText("My dream is to travel everywhere, much like you. ");
	addText("My father, however, does not allow me to do it, because he thinks it's very dangerous. ");
	addText("He may say yes, though, if I show him some sort of proof that I'm not the weak girl that he thinks I am.");
	sendOk();
end