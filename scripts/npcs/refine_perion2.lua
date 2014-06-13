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
-- Mr. Smith - Item Creator (Perion)

dofile("scripts/utils/itemProductionHelper.lua");
dofile("scripts/utils/npcHelper.lua");

function makeItemChoice(itemId, levelLimit, reqs, jobType)
	return makeChoiceData(" " .. itemRef(itemId) .. black("(level limit: " .. levelLimit .. ", " .. jobType .. ")", previousBlue), {itemId, levelLimit, reqs});
end

addText("I am Mr. Thunder's apprentice. ");
addText("He's gettin up there with age, and he isn't what he used to be...haha, oh crap, please don't tell him that I said that...");
addText("Anyway...I can make various items specifically designed for the warriors, so what do you think? ");
addText("Wanna leave it up to me?");
answer = askYesNo();

if answer == answer_no then
	addText("Sigh ... ");
	addText("I'll definitely hear it from my boss if I don't make the norm today ... ");
	addText("Oh well, that sucks.");
	sendNext();
else
	choices = {
		makeChoiceHandler(" Make a glove", function()
			choices = {
				makeItemChoice(1082003, 10, {4000021, 15, 4011001, 1, item_mesos, 1000}, "warrior"),
				makeItemChoice(1082000, 15, {4011001, 2, item_mesos, 2000}, "warrior"),
				makeItemChoice(1082004, 20, {4000021, 40, 4011000, 2, item_mesos, 5000}, "warrior"),
				makeItemChoice(1082001, 25, {4011001, 2, item_mesos, 10000}, "warrior"),
				makeItemChoice(1082007, 30, {4011000, 3, 4003000, 15, item_mesos, 20000}, "warrior"),
				makeItemChoice(1082008, 35, {4000021, 30, 4011001, 4, 4003000, 30, item_mesos, 30000}, "warrior"),
				makeItemChoice(1082023, 40, {4000021, 50, 4011001, 5, 4003000, 40, item_mesos, 40000}, "warrior"),
				makeItemChoice(1082009, 50, {4011001, 3, 4021007, 2, 4000030, 30, 4003000, 45, item_mesos, 50000}, "warrior"),
				makeItemChoice(1082059, 60, {4011007, 1, 4011000, 8, 4011006, 2, 4000030, 50, 4003000, 50, item_mesos, 70000}, "warrior"),
			};

			addText("I'm the best glove-maker in this town!! ");
			addText("Now...what kind of a glove do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", I need the following items. ");
			addText("The level limit is " .. levelLimit .. " and please make sure you don't use an item that's been upgraded as a material for it. ");
			addText("What do you think? ");
			addText("Do you want one?\r\n");
			displayResources(reqs);
			answer = askYesNo();

			if answer == answer_no then
				addText("Lacking the materials? ");
				addText("It's ok ... collect them all and then come find me, alright? ");
				addText("I'll be waiting...");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, 1) then
					addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				else
					addText("Here! ");
					addText("take the " .. itemRef(itemId) .. ". ");
					addText("Don't you think I'm as good as Mr. Thunder? ");
					addText("You'll be more than satisfied with what I made here.");
					takeResources(reqs);
					giveItem(itemId, 1);
				end
				sendNext();
			end
		end),
		makeChoiceHandler(" Upgrade a glove", function()
			choices = {
				makeItemChoice(1082005, 30, {1082007, 1, 4011001, 1, item_mesos, 20000}, "warrior"),
				makeItemChoice(1082006, 30, {1082007, 1, 4011005, 2, item_mesos, 25000}, "warrior"),
				makeItemChoice(1082035, 35, {1082008, 1, 4021006, 3, item_mesos, 30000}, "warrior"),
				makeItemChoice(1082036, 35, {1082008, 1, 4021008, 1, item_mesos, 40000}, "warrior"),
				makeItemChoice(1082024, 40, {1082023, 1, 4011003, 4, item_mesos, 45000}, "warrior"),
				makeItemChoice(1082025, 40, {1082023, 1, 4021008, 2, item_mesos, 50000}, "warrior"),
				makeItemChoice(1082010, 50, {1082009, 1, 4011002, 5, item_mesos, 55000}, "warrior"),
				makeItemChoice(1082011, 50, {1082009, 1, 4011006, 4, item_mesos, 60000}, "warrior"),
				makeItemChoice(1082060, 60, {1082059, 1, 4011002, 3, 4021005, 5, item_mesos, 70000}, "warrior"),
				makeItemChoice(1082061, 60, {1082059, 1, 4021007, 2, 4021008, 2, item_mesos, 80000}, "warrior"),
			};

			addText("So you want to upgrade the glove? ");
			addText("Ok, then. ");
			addText("A word of warning, though: All the items that will be used for upgrading will be gone, but if you use an item that has been " .. red("upgraded") .. " with a scroll, the effect will disappear when upgraded. ");
			addText("Please take that into consideration when making the decision, ok?");
			sendNext();

			addText("So~~ what kind of a glove do you want to upgrade and create?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", I need the following items. ");
			addText("The level limit is " .. levelLimit .. " and please make sure you don't use an item that's been upgraded as a material for it. ");
			addText("What do you think? ");
			addText("Do you want one?\r\n");
			displayResources(reqs);
			answer = askYesNo();

			if answer == answer_no then
				addText("Lacking the materials? ");
				addText("It's ok ... collect them all and then come find me, alright? ");
				addText("I'll be waiting...");
				sendNext();
			else
				if not hasResources(reqs) or not hasOpenSlotsFor(itemId, 1) then
					addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				else
					addText("Here! ");
					addText("take the " .. itemRef(itemId) .. ". ");
					addText("Don't you think I'm as good as Mr. Thunder? ");
					addText("You'll be more than satisfied with what I made here.");
					takeResources(reqs);
					giveItem(itemId, 1);
				end
				sendNext();
			end
		end),
		makeChoiceHandler(" Create materials", function()
			multiplier = nil;
			choices = {
				makeChoiceHandler("Make " .. itemRef(4003001) .. " with " .. itemRef(4000003), function()
					addText("With " .. blue("10 " .. itemRef(4000003) .. "es") .. ", I can make 1 " .. itemRef(4003001) .. "(s). ");
					addText("Be thankful, because this one's on me. ");
					addText("What do you think? ");
					addText("How many do you want?");
					multiplier = askNumber(0, 0, 100);

					addText("You want to make " .. blue(itemRef(4003001) .. "(s)") .. " " .. multiplier .. " time(s)? ");
					addText("I'll need  " .. red((10 * multiplier) .. " " .. itemRef(4000003) .. "es") .. " then.");
					return {4003001, 1, {4000003, 10}};
				end),
				makeChoiceHandler("Make " .. itemRef(4003001) .. " with " .. itemRef(4000018), function()
					addText("With " .. blue("5 " .. itemRef(4000018) .. "s") .. ", I can make 1 " .. itemRef(4003001) .. "(s). ");
					addText("Be thankful, because this one's on me. ");
					addText("What do you think? ");
					addText("How many do you want?");
					multiplier = askNumber(0, 0, 100);

					addText("You want to make " .. blue(itemRef(4003001) .. "(s)") .. " " .. multiplier .. " time(s)? ");
					addText("I'll need  " .. red((5 * multiplier) .. " " .. itemRef(4000018) .. "s") .. " then.");
					return {4003001, 1, {4000018, 5}};
				end),
				makeChoiceHandler("Make " .. itemRef(4003000) .. "s", function()
					addText("With " .. blue("1 " .. itemRef(4011001) .. "(s) and " .. itemRef(4011000) .. "(s) each") .. ", I can make 15 " .. itemRef(4003000) .. "s. ");
					addText("Be thankful, because this one's on me. ");
					addText("What do you think? How many do you want?");
					multiplier = askNumber(0, 0, 100);

					addText("You want to make " .. blue(itemRef(4003000) .. "s") .. " " .. multiplier .. " time(s)? ");
					addText("I'll need  " .. red(multiplier .. " " .. itemRef(4011001) .. "(s) and " .. itemRef(4011000) .. "(s) each") .. " then.");
					return {4003000, 15, {4011000, 1, 4011001, 1}};
				end),
			};

			addText("So you want to make some materials, huh? ");
			addText("Okay...what kind of materials do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, qty, reqs = data[1], data[2], data[3];

			answer = askYesNo();
			if multiplier == 0 or not hasResources(reqs, multiplier) or not hasOpenSlotsFor(itemId, qty * multiplier) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				sendNext();
			else
				takeResources(reqs, multiplier);
				giveItem(itemId, qty * multiplier);
				addText("Here! ");
				addText("take " .. qty * multiplier  .. " " .. itemRef(itemId) .. "(s). ");
				addText("Don't you think I'm as good as Mr. Thunder? ");
				addText("You'll be more than satisfied with what I made here.");
				sendNext();
			end
		end),
	};

	addText("Alright! ");
	addText("The service fee will be reasonable so don't worry about it. ");
	addText("What do you want to do?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
end