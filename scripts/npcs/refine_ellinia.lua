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
-- Francois - Item Creator (Ellinia)

dofile("scripts/utils/itemProductionHelper.lua");
dofile("scripts/utils/npcHelper.lua");

function makeItemChoice(itemId, levelLimit, reqs, jobType, statText)
	return makeChoiceData(" " .. itemRef(itemId) .. black("(Level limit : " .. levelLimit .. ", " .. jobType .. ")", npc_text_blue), {itemId, levelLimit, reqs, statText});
end

addText("Do you want to take a look at some items? ");
addText("Well... any thought of making one? ");
addText("I'm actually a wizard that was banished from the town because I casted an illegal magic. ");
addText("Because of that I've been hiding, doing some business here... well, that's not really the point. ");
addText("Do you want to do some business with me?");
answer = askYesNo();

if answer == answer_no then
	addText("You don't trust my skills, I suppose... haha... you should know that I used to be a great wizard. ");
	addText("You still can't believe my skills, huh... but just remember that I used to be the great magician of old...");
	sendNext();
else
	choices = {
		makeChoiceHandler(" Make a wand", function()
			choices = {
				makeItemChoice(1372005, 8, {4003001, 5, item_mesos, 1000}, "all"),
				makeItemChoice(1372006, 13, {4003001, 10, 4000001, 50, item_mesos, 3000}, "all"),
				makeItemChoice(1372002, 18, {4011001, 1, 4000009, 30, 4003000, 5, item_mesos, 5000}, "magician"),
				makeItemChoice(1372004, 23, {4011002, 2, 4003002, 1, 4003000, 10, item_mesos, 12000}, "magician"),
				makeItemChoice(1372003, 28, {4011002, 3, 4021002, 1, 4003000, 10, item_mesos, 30000}, "magician"),
				makeItemChoice(1372001, 33, {4021006, 5, 4011002, 3, 4011001, 1, 4003000, 15, item_mesos, 60000}, "magician"),
				makeItemChoice(1372000, 38, {4021006, 5, 4021005, 5, 4021007, 1, 4003003, 1, 4003000, 20, item_mesos, 120000}, "magician"),
				makeItemChoice(1372007, 48, {4011006, 4, 4021003, 3, 4021007, 2, 4021002, 1, 4003002, 1, 4003000, 30, item_mesos, 200000}, "magician"),
			};

			addText("If you gather up the materials for me, I'll make a wand for you with my magical power. ");
			addText("How... what kind of a wand do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", you'll need these items below. ");
			addText("The level limit for the item will be " .. levelLimit .. " so please check and see if you really need the item, first of all. ");
			addText("Are you sure you want to make one?\r\n\r\n");
			return data;
		end),
		makeChoiceHandler(" Make a staff", function()
			choices = {
				makeItemChoice(1382000, 10, {4003001, 5, item_mesos, 2000}, "magician"),
				makeItemChoice(1382003, 15, {4021005, 1, 4011001, 1, 4003000, 5, item_mesos, 2000}, "magician"),
				makeItemChoice(1382005, 15, {4021003, 1, 4011001, 1, 4003000, 5, item_mesos, 2000}, "magician"),
				makeItemChoice(1382004, 20, {4003001, 50, 4011001, 1, 4003000, 10, item_mesos, 5000}, "magician"),
				makeItemChoice(1382002, 25, {4021006, 2, 4021001, 1, 4011001, 1, 4003000, 15, item_mesos, 12000}, "magician"),
				makeItemChoice(1382001, 45, {4011001, 8, 4021006, 5, 4021001, 5, 4021005, 5, 4003000, 30, 4000010, 50, 4003003, 1, item_mesos, 180000}, "magician"),
			};

			addText("If you gather up the materials for me, I'll make a staff for you with my magical power. ");
			addText("How... what kind of a wand do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", you'll need these items below. ");
			addText("The level limit for the item will be " .. levelLimit .. " so please check and see if you really need the item, first of all. ");
			addText("Are you sure you want to make one?\r\n\r\n");
			return data;
		end),
		makeChoiceHandler(" Make a glove", function()
			choices = {
				makeItemChoice(1082019, 15, {4000021, 15, item_mesos, 500}, "magician"),
				makeItemChoice(1082020, 20, {4000021, 30, 4011001, 1, item_mesos, 300}, "magician"),
				makeItemChoice(1082026, 25, {4000021, 50, 4011006, 2, item_mesos, 500}, "magician"),
				makeItemChoice(1082051, 30, {4000021, 60, 4021006, 1, 4021000, 2, item_mesos, 800}, "magician"),
				makeItemChoice(1082054, 35, {4000021, 70, 4011006, 1, 4011001, 3, 4021000, 2, item_mesos, 500}, "magician"),
				makeItemChoice(1082062, 40, {4000021, 80, 4021000, 3, 4021006, 3, 4003000, 30, item_mesos, 800}, "magician"),
				makeItemChoice(1082081, 50, {4021000, 3, 4011006, 2, 4000030, 55, 4003000, 40, item_mesos, 1000}, "magician", "INT + 1"),
				makeItemChoice(1082086, 60, {4011007, 1, 4011001, 8, 4021007, 1, 4000030, 50, 4003000, 50, item_mesos, 1500}, "magician", "INT + 1, LUK + 1"),
			};

			addText("If you gather up the materials for me, I'll make a glove for you with my magical power. ");
			addText("How... what kind of a wand do you want to make?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs = data[1], data[2], data[3];

			addText("To make one " .. itemRef(itemId) .. ", you'll need these items below. ");
			addText("The level limit for the item will be " .. levelLimit .. " so please check and see if you really need the item, first of all. ");
			addText("Are you sure you want to make one?\r\n\r\n");
			return data;
		end),
		makeChoiceHandler(" Upgrade a glove", function()
			choices = {
				makeItemChoice(1082021, 20, {1082020, 1, 4011001, 1, item_mesos, 20000}, "magician", "INT + 1"),
				makeItemChoice(1082022, 20, {1082020, 1, 4021001, 2, item_mesos, 25000}, "magician", "INT + 2"),
				makeItemChoice(1082027, 25, {1082026, 1, 4021000, 3, item_mesos, 30000}, "magician", "INT + 1"),
				makeItemChoice(1082028, 25, {1082026, 1, 4021008, 1, item_mesos, 40000}, "magician", "INT + 2"),
				makeItemChoice(1082052, 30, {1082051, 1, 4021005, 3, item_mesos, 35000}, "magician", "INT + 1"),
				makeItemChoice(1082053, 30, {1082051, 1, 4021008, 1, item_mesos, 40000}, "magician", "INT + 2"),
				makeItemChoice(1082055, 35, {1082054, 1, 4021005, 3, item_mesos, 40000}, "magician", "INT + 1"),
				makeItemChoice(1082056, 35, {1082054, 1, 4021008, 1, item_mesos, 45000}, "magician", "INT + 2"),
				makeItemChoice(1082063, 40, {1082062, 1, 4021002, 4, item_mesos, 45000}, "magician", "INT + 2"),
				makeItemChoice(1082064, 40, {1082062, 1, 4021008, 2, item_mesos, 50000}, "magician", "INT + 3"),
				makeItemChoice(1082082, 50, {1082081, 1, 4021002, 5, item_mesos, 55000}, "magician", "INT + 2, MP + 15"),
				makeItemChoice(1082080, 50, {1082081, 1, 4021008, 3, item_mesos, 60000}, "magician", "INT + 3, MP + 30"),
				makeItemChoice(1082087, 60, {1082086, 1, 4011004, 3, 4011006, 5, item_mesos, 70000}, "magician", "INT + 2, LUK + 1, MP + 15"),
				makeItemChoice(1082088, 60, {1082086, 1, 4021008, 2, 4011006, 3, item_mesos, 80000}, "magician", "INT + 3, LUK + 1, MP + 30"),
			};

			addText("So you want to upgrade a glove? ");
			addText("Be careful, though; ");
			addText("All the items that will be used for upgrading will be gone, and if you use an item that has been " .. red("upgraded") .." with a scroll, the effect will disappear when upgraded, so you may want to think about it before making your decision ...");
			sendNext();

			addText("Now .. which glove do you want to upgrade?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs, statText = data[1], data[2], data[3], data[4];

			addText("To upgrade one " .. itemRef(itemId) .. ", you'll need these items below. ");
			addText("The level limit for the item is " .. levelLimit .. ", ");
			if statText ~= nil then
				addText("with the item option of " .. red(statText) .. " attached to it, ");
			end
			addText("so please check and see if you really need it. ");
			addText("Oh, and one thing. ");
			addText("Please make sure NOT to use an upgraded item as a material for the upgrade. ");
			addText("Now, are you sure you want to make this item?\r\n\r\n");
			return data;
		end),
		makeChoiceHandler(" Upgrade a hat", function()
			choices = {
				makeItemChoice(1002065, 30, {1002064, 1, 4011001, 3, item_mesos, 40000}, "wizard", "INT + 1"),
				makeItemChoice(1002013, 30, {1002064, 1, 4011006, 3, item_mesos, 50000}, "wizard", "INT + 2"),
			};

			addText("So you want to upgrade a hat ... ");
			addText("Be careful, though; ");
			addText("All the items that will be used for upgrading will be gone, and if you use an item that has been " .. red("upgraded") .. " with a scroll, the effect will disappear when upgraded, so you may want to think about it before making your decision ...");
			sendNext();

			addText("Alright, so which hat would you like to upgrade?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			data = selectChoice(choices, choice);
			itemId, levelLimit, reqs, statText = data[1], data[2], data[3], data[4];

			addText("To upgrade one " .. itemRef(itemId) .. ", you'll need these items below.");
			addText("The level limit for the item is " .. levelLimit .. ", ");
			if statText ~= nil then
				addText("with the item option of " .. red(statText) .. " attached to it, ");
			end
			addText("so please check and see if you really need it. ");
			addText("Oh, and one thing. ");
			addText("Please make sure NOT to use an upgraded item as a material for the upgrade. ");
			addText("Now, are you sure you want to make this item?\r\n\r\n");
			return data;
		end),
	};

	addText("Alright ... it's for both of our own good, right? ");
	addText("Choose what you want to do...\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	itemId, levelLimit, reqs = data[1], data[2], data[3];

	displayResources(reqs);
	answer = askYesNo();
	if answer == answer_no then
		addText("Really? ");
		addText("You must be lacking materials. ");
		addText("Try harder at gathering them up around town. ");
		addText("Fortunately it looks like the monsters around the forest have various materials on their sleeves.");
		sendNext();
	else
		if not hasResources(reqs) or not hasOpenSlotsFor(itemId, 1) then
			addText("Please check and see if you have all the items you need, or if your equipment inventory is full or not.");
			sendNext();
		else
			addText("Here, take " .. itemRef(itemId) .. ". ");
			addText("The more I see it, the more it looks perfect. ");
			addText("Hahah, it's not a stretch to think that other magicians fear my skills ...");
			sendNext();

			takeResources(reqs);
			giveItem(itemId, 1);
		end
	end
end