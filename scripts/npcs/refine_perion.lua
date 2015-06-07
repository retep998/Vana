--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Mr. Thunder - Item Creator (Perion)

dofile("scripts/utils/itemProductionHelper.lua");
dofile("scripts/utils/npcHelper.lua");

function makeOreChoice(itemId, reqs)
	return makeChoiceData(" " .. itemRef(itemId), {itemId, reqs});
end

function makeItemChoice(itemId, levelLimit, reqs, jobType, statText)
	return makeChoiceData(" " .. itemRef(itemId) .. black("(level limit: " .. levelLimit .. ", " .. jobType .. ")", npc_text_blue), {itemId, reqs, statText});
end

addText("Wait, do you have the ore of either a jewel or mineral? ");
addText("With a little service fee, I can turn them into materials needed to create weapons or shields. ");
addText("Not only that, I can also upgrade an item with it to make an even better item. ");
addText("What do you think? ");
addText("Do you want to do it?");
answer = askYesNo();

if answer == answer_no then
	addText("Really? ");
	addText("Sorry to hear that. ");
	addText("If you don't need it, then oh well ... if you happen to collect a lot of ores in the future, please find me. ");
	addText("I'll make something only I can make.");
	sendNext();
else
	choices = {
		makeChoiceHandler(" Refine the raw ore of a mineral", function()
			choices = {
				makeOreChoice(4011000, {4010000, 10, item_mesos, 300}),
				makeOreChoice(4011001, {4010001, 10, item_mesos, 300}),
				makeOreChoice(4011002, {4010002, 10, item_mesos, 300}),
				makeOreChoice(4011003, {4010003, 10, item_mesos, 500}),
				makeOreChoice(4011004, {4010004, 10, item_mesos, 500}),
				makeOreChoice(4011005, {4010005, 10, item_mesos, 500}),
				makeOreChoice(4011006, {4010006, 10, item_mesos, 800}),
			};

			addText("Which mineral do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, reqs = data[1], data[2];

			addText("To make a(n) " .. itemRef(itemId) .. ", I need the following materials. ");
			addText("How many would you like to make?\r\n\r\n");
			displayResources(reqs);
			amount = askNumber(0, 0, 100);

			addText("To make " .. blue(amount .. " " .. itemRef(itemId) .. "(s)") .. ", I need the following materials. ");
			addText("Are you sure you want to make these?\r\n\r\n");
			displayResources(reqs, amount);
			answer = askYesNo();

			if answer == answer_no then
				addText("We have all kinds of items so don't panic, and choose the one you want to buy...");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, amount) or amount == 0 then
					addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
				else
					addText("Hey! ");
					addText("Here, take " .. amount .. " " .. itemRef(itemId) .. "(s). ");
					addText("This came out even finer than I though ... a finely refined item like this, I don't think you'll see it anywhere else!! ");
					addText("Please come again~");
					takeResources(reqs, amount);
					giveItem(itemId, amount);
				end
				sendNext();
			end
		end),
		makeChoiceHandler(" Refine a jewel ore", function()
			choices = {
				makeOreChoice(4021000, {4020000, 10, item_mesos, 500}),
				makeOreChoice(4021001, {4020001, 10, item_mesos, 500}),
				makeOreChoice(4021002, {4020002, 10, item_mesos, 500}),
				makeOreChoice(4021003, {4020003, 10, item_mesos, 500}),
				makeOreChoice(4021004, {4020004, 10, item_mesos, 500}),
				makeOreChoice(4021005, {4020005, 10, item_mesos, 500}),
				makeOreChoice(4021006, {4020006, 10, item_mesos, 500}),
				makeOreChoice(4021007, {4020007, 10, item_mesos, 1000}),
				makeOreChoice(4021008, {4020008, 10, item_mesos, 3000}),
			};

			addText("Which jewel do you want to refine?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, reqs = data[1], data[2];

			addText("To make a(n) " .. itemRef(itemId) .. ", I need the following materials. ");
			addText("How many would you like to make?\r\n\r\n");
			displayResources(reqs);
			amount = askNumber(0, 0, 100);

			addText("To make " .. blue(amount .. " " .. itemRef(itemId) .. "(s)") .. ", I need the following materials. ");
			addText("Are you sure you want to make these?\r\n\r\n");
			displayResources(reqs, amount);
			answer = askYesNo();

			if answer == answer_no then
				addText("We have all kinds of items so don't panic, and choose the one you want to buy...");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, amount) or amount == 0 then
					addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
				else
					addText("Hey! Here, take " .. amount .. " " .. itemRef(itemId) .. "(s). ");
					addText("This came out even finer than I though ... a finely refined item like this, I don't think you'll see it anywhere else!! ");
					addText("Please come again~");
					takeResources(reqs, amount);
					giveItem(itemId, amount);
				end
				sendNext();
			end
		end),
		makeChoiceHandler(" Upgrade a helmet", function()
			choices = {
				makeItemChoice(1002042, 15, {1002001, 1, 4011002, 1, item_mesos, 500}, "all"),
				makeItemChoice(1002041, 15, {1002001, 1, 4021006, 1, item_mesos, 300}, "all"),
				makeItemChoice(1002002, 10, {1002043, 1, 4011001, 1, item_mesos, 500}, "warrior"),
				makeItemChoice(1002044, 10, {1002043, 1, 4011002, 1, item_mesos, 800}, "warrior"),
				makeItemChoice(1002003, 12, {1002039, 1, 4011001, 1, item_mesos, 500}, "warrior"),
				makeItemChoice(1002040, 12, {1002039, 1, 4011002, 1, item_mesos, 800}, "warrior"),
				makeItemChoice(1002007, 15, {1002051, 1, 4011001, 2, item_mesos, 1000}, "warrior"),
				makeItemChoice(1002052, 15, {1002051, 1, 4011002, 2, item_mesos, 1500}, "warrior"),
				makeItemChoice(1002011, 20, {1002059, 1, 4011001, 3, item_mesos, 1500}, "warrior"),
				makeItemChoice(1002058, 20, {1002059, 1, 4011002, 3, item_mesos, 2000}, "warrior"),
				makeItemChoice(1002009, 20, {1002055, 1, 4011001, 3, item_mesos, 1500}, "warrior"),
				makeItemChoice(1002056, 20, {1002055, 1, 4011002, 3, item_mesos, 2000}, "warrior"),
				makeItemChoice(1002087, 22, {1002027, 1, 4011002, 4, item_mesos, 2000}, "warrior"),
				makeItemChoice(1002088, 22, {1002027, 1, 4011006, 4, item_mesos, 4000}, "warrior"),
				makeItemChoice(1002049, 25, {1002005, 1, 4011006, 5, item_mesos, 4000}, "warrior"),
				makeItemChoice(1002050, 25, {1002005, 1, 4011005, 5, item_mesos, 5000}, "warrior"),
				makeItemChoice(1002047, 35, {1002004, 1, 4021000, 3, item_mesos, 8000}, "warrior", "HP + 10"),
				makeItemChoice(1002048, 35, {1002004, 1, 4021005, 3, item_mesos, 10000}, "warrior", "DEX + 1"),
				makeItemChoice(1002099, 40, {1002021, 1, 4011002, 5, item_mesos, 12000}, "warrior", "STR + 1"),
				makeItemChoice(1002098, 40, {1002021, 1, 4011006, 6, item_mesos, 15000}, "warrior", "STR + 2"),
				makeItemChoice(1002085, 50, {1002086, 1, 4011002, 5, item_mesos, 20000}, "warrior", "STR + 1"),
				makeItemChoice(1002028, 50, {1002086, 1, 4011004, 4, item_mesos, 25000}, "warrior", "STR + 2"),
				makeItemChoice(1002022, 55, {1002100, 1, 4011007, 1, 4011001, 7, item_mesos, 30000}, "warrior", "DEX + 1, MP + 30"),
				makeItemChoice(1002101, 55, {1002100, 1, 4011007, 1, 4011002, 7, item_mesos, 30000}, "warrior", "STR + 1, MP + 30"),
			};

			addText("So you want to upgrade the helmet? ");
			addText("Ok, then. ");
			addText("A word of warning, though: All the items that will be used for upgrading will be gone, and if you use an item that has been " .. red("upgraded") .. " with a scroll before, the effect will not go in when upgraded. ");
			addText("Please take that info consideration when making the decision, ok?");
			sendNext();

			addText("So~~ what kind of a helmet do you want to upgrade and create?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, reqs, statText = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", I need the following materials. ");
			if statText ~= nil then
				addText("This item has an option of " .. statText .. ". ");
			end
			addText("Make sure you don't use an item that's been upgraded as a material for it. ");
			addText("What do you think? ");
			addText("Do you want one?\r\n\r\n");
			displayResources(reqs);
			answer = askYesNo();

			if answer == answer_no then
				addText("Really? ");
				addText("Sorry to hear that. ");
				addText("Come back when you need me.");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, 1) then
					addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
				else
					addText("Hey! ");
					addText("Here, take " .. itemRef(itemId) .. ". ");
					addText("I'm good ... a finely refined item like this, have you seen it anywhere else?? ");
					addText("Please come again~");
					takeResources(reqs);
					giveItem(itemId, 1);
				end
				sendNext();
			end
		end),
		makeChoiceHandler(" Upgrade a shield", function()
			choices = {
				makeItemChoice(1092013, 40, {1092012, 1, 4011002, 10, item_mesos, 100000}, "warrior", "STR + 2"),
				makeItemChoice(1092014, 40, {1092012, 1, 4011003, 10, item_mesos, 100000}, "warrior", "DEX + 2"),
				makeItemChoice(1092010, 60, {1092009, 1, 4011007, 1, 4011004, 15, item_mesos, 120000}, "warrior", "DEX + 2"),
				makeItemChoice(1092011, 60, {1092009, 1, 4011007, 1, 4011003, 15, item_mesos, 120000}, "warrior", "STR + 2"),
			};

			addText("So you want to upgrade the shield? ");
			addText("Ok, then. ");
			addText("A word of warning, though: All the items that will be used for upgrading will be gone, and if you use an item that has been " .. red("upgraded") .. " with a scroll before, the effect will not go in when upgraded. ");
			addText("Please take that info consideration when making the decision, ok?");
			sendNext();

			addText("So~~ what kind of a shield do you want to upgrade and create?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, reqs, statText = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", I need the following materials. ");
			if statText ~= nil then
				addText("This item has an option of " .. statText ..". ");
			end
			addText("Make sure you don't use an item that's been upgraded as a material for it. ");
			addText("What do you think? ");
			addText("Do you want one?\r\n\r\n");
			displayResources(reqs);
			answer = askYesNo();

			if answer == answer_no then
				addText("Really? ");
				addText("Sorry to hear that. ");
				addText("Come back when you need me.");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, 1) then
					addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
				else
					addText("Hey! ");
					addText("Here, take " .. itemRef(itemId) .. ". ");
					addText("I'm good ... a finely refined item like this, have you seen it anywhere else?? ");
					addText("Please come again~");
					takeResources(reqs);
					giveItem(itemId, 1);
				end
				sendNext();
			end
		end),
	};

	addText("Alright, with the ore and a little service fee, I'll refine it so you can you use it. ");
	addText("Check and see if your etc, storage has any room. ");
	addText("Now ... what would you like me to do?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
end