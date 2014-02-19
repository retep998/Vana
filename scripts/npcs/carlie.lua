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
-- Staff Sergeant Charlie, exchange quest

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/itemProduction.lua");

function generateChoice(itemId, rewards)
	return makeChoiceData(" 100 " .. itemRef(itemId) .. "s", {itemId, rewards});
end

choices = {
	generateChoice(4000073, {{2000001, 20},	{2010004, 10}, {2000003, 15}, {4003001, 15}, {2020001, 15}, {2030000, 15}}), -- Solid Horn
	generateChoice(4000059, {{2000001, 30},	{2010001, 40}, {2000003, 20}, {4003001, 20}, {2040002, 01}}), -- Star Pixie's Starpiece
	generateChoice(4000076, {{2000001, 30},	{2010001, 40}, {2000003, 20}, {4003001, 20}, {2040002, 01}}), -- Fly-Eye Wings
	generateChoice(4000058, {{2000002, 15},	{2010004, 15}, {2000003, 25}, {4003001, 30}, {2040302, 01}}), -- Nependeath Seed
	generateChoice(4000078, {{2000002, 15},	{2010004, 15}, {2000003, 25}, {2050004, 15}, {4003001, 30}, {2040302, 01}}), -- Jr. Cerebes Tooth
	generateChoice(4000060, {{2000002, 25},	{2000006, 10}, {2022000, 05}, {4000030, 15}, {2040902, 01}}), -- Lunar Pixie's Moonpiece
	generateChoice(4000062, {{2000002, 30},	{2000006, 15}, {2020000, 20}, {4003000, 05}, {2040402, 01}}), -- Dark Nependeath Seed
	generateChoice(4000048, {{2000002, 30},	{2000006, 15}, {2020000, 20}, {4003000, 05}, {2040402, 01}}), -- Jr. Yeti Skin
	generateChoice(4000081, {{2000006, 25},	{2020006, 25}, {4010004, 08}, {4010005, 08}, {4010006, 03}, {4020007, 02}, {4020008, 02}, {2040705, 01}}), -- Firebomb Flame
	generateChoice(4000061, {{2000002, 30}, {2000006, 15}, {2020000, 20}, {4003000, 05}, {2041016, 01}}), -- Luster Pixie's Sunpiece
	generateChoice(4000070, {{2000002, 30}, {2000006, 15}, {2020000, 20}, {4003000, 05}, {2041005, 01}}), -- Cellion Tail
	generateChoice(4000071, {{2000002, 30}, {2000006, 15}, {2020000, 20}, {4003000, 05}, {2041005, 01}}), -- Lioner Tail
	generateChoice(4000072, {{2000002, 30},	{2000006, 15}, {2020000, 20}, {4003000, 05}, {2041005, 01}}), -- Grupin Tail
	generateChoice(4000051, {{2002004, 15}, {2002005, 15}, {2002003, 10}, {4001005, 01}, {2040502, 01}}), -- Hector Tail
	generateChoice(4000055, {{2002005, 30}, {2002006, 15}, {2022001, 30}, {4003003, 01}, {2040505, 01}}), -- Dark Jr. Yeti Skin
	generateChoice(4000069, {{2002005, 30}, {2002006, 15}, {2000006, 20}, {2050004, 20}, {4003003, 01}, {2041002, 01}}), -- Zombie's Lost Tooth
	generateChoice(4000052, {{2000006, 20}, {4010004, 07}, {4010005, 07}, {4010003, 07}, {4003002, 01}, {2040602, 01}}), -- White Pang Tail
	generateChoice(4000050, {{2000006, 20}, {4010000, 07}, {4010001, 07}, {4010002, 07}, {4010006, 02}, {4003000, 05}, {2040702, 01}}), -- Pepe Beak
	generateChoice(4000057, {{2000006, 20}, {4010004, 07}, {4010005, 07}, {4010006, 03}, {4020007, 02}, {4020008, 02}, {2040705, 01}}), -- Dark Pepe Beak
	generateChoice(4000049, {{2000006, 25}, {2020000, 20}, {4020000, 07}, {4020001, 07}, {4020002, 03}, {4020007, 02}, {2040708, 01}}), -- Yeti Horn
	generateChoice(4000056, {{2000006, 25}, {4020005, 07}, {4020003, 07}, {4020004, 07}, {4020008, 02}, {2040802, 01}}), -- Dark Yeti Horn
	generateChoice(4000079, {{2000006, 25}, {2050004, 30}, {2022001, 35}, {4020000, 07}, {4020001, 07}, {4020002, 07}, {4020007, 02}, {2041023, 01}}), -- Cerebes Tooth
	generateChoice(4000053, {{2000006, 30}, {4020006, 07}, {4020007, 02}, {4020008, 02}, {2070010, 01}, {2040805, 01}}), -- Werewolf Toenail
	generateChoice(4000054, {{2000006, 30}, {4020006, 07}, {4020007, 02}, {4020008, 02}, {2041020, 01}}), -- Lycanthrope Toenail
	generateChoice(4000080, {{2000006, 35}, {4020006, 09}, {4020007, 04}, {4020008, 04}, {2041008, 01}}), -- Bain's Spiky Collar
};

addText("Hey, got a little bit of time? ");
addText("Well, my job is to collect items here and sell them elsewhere, but these days the monsters have become much more hostile so it's been difficult getting good items ... ");
addText("What do you think? ");
addText("Do you want to do some business with me?");
sendNext();

addText("The deal is simple. ");
addText("You get me something I need, I get you something you need. ");
addText("The problem is, I deal with a whole bunch of people, so the items I have to offer may change every time you see me. ");
addText("What do you think? ");
addText("Still want to do it?");
answer = askYesNo();

if answer == answer_no then
	addText("Hmmm...it shouldn't be a bad deal for you. ");
	addText("Come see me at the right time and you may get a much better item to be offered. ");
	addText("Anyway, let me know when you have a change of heart.");
	sendNext();
else
	addText("Ok! First you need to choose the item that you'll trade with. ");
	addText("The better the item, the more likely the chance that I'll give you something much nicer in return.\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	data = selectChoice(choices, choice);
	itemId = data[1];
	rewards = data[2];
	qty = 100;
	requirements = {itemId, qty};

	addText("Let's see, you want to trade your " .. blue(qty .. " " .. itemRef(itemId) .. "s") .. " with my stuff, right? ");
	addText("Before trading make sure you have an empty slot available on your use or etc. inventory. ");
	addText("Now, do you really want to trade with me?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Hmmm ... it shouldn't be a bad deal for you at all. ");
		addText("If you come at the right time I can hook you up with good items. ");
		addText("Anyway if you feel like trading, feel free to come.");
		sendNext();
	else
		reward = rewards[getRandomNumber(#rewards)];
		if getOpenSlots(4) == 0 or getOpenSlots(2) == 0 or not hasResources(requirements, 1) then
			addText("Hmmm... are you sure you have " .. blue(qty .. " " .. itemRef(itemId) .. "s") .. "? ");
			addText("If so, then please check and see if your item inventory is full or not.");
			sendNext();
		else
			giveExp(500 * getQuestExpRate());
			takeResources(requirements, 1);
			giveItem(reward[1], reward[2]);

			addText("For your " .. blue(qty .. " " .. itemRef(itemId) .. "s") .. ", here's my " .. blue(reward[2] .. " " .. itemRef(reward[1]) .. "(s)") .. ". ");
			addText("What do you think? ");
			addText("Do you like the items I gave you in return? ");
			addText("I plan on being here for a while, so if you gather up more items, I'm always open for a trade ...");
			sendNext();
		end
	end
end