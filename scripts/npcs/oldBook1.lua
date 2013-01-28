--[[
Copyright (C) 2008-2013 Vana Development Team

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

if isQuestCompleted(3035) then
	local items = {
		{2050003, 300, " that cures the state of being sealed and cursed"},
		{2050004, 400, " that cures all"},
		{4006000, 5000, ", possessing magical power, that is used for high-quality skills"},
		{4006001, 5000, ", possessing the power of summoning that is used for high-quality skills"}
	};

	addText("Thanks to you #b#t4031056##k is safely sealed. ");
	addText("Of course, also as a result, I used up about half of the power I have accumulated over the last 800 years or so...but now I can die in peace. ");
	addText("Oh, by the way... are you looking for rare items by any chance? ");
	addText("As a sign of appreciation for your hard work, I'll sell some items I have to you, and ONLY you. ");
	addText("Pick out the one you want!\r\n");
	addText("#L0# #b#t2050003#(Price : 300 mesos)#k#l\r\n");
	addText("#L1# #b#t2050004#(Price : 400 mesos)#k#l\r\n");
	addText("#L2# #b#t4006000#(Price : 5000 mesos)#k#l\r\n");
	addText("#L3# #b#t4006001#(Price : 5000 mesos)#k#l\r\n");
	choice = askChoice() + 1;

	local itemId = items[choice][1];
	local itemPrice = items[choice][2];
	local itemDesc = items[choice][3];

	addText("Is #b#t" .. itemId .. "##k really the item that you need? ");
	addText("It's the item" .. itemDesc .. ". ");
	addText("It may not be the easiest item to acquire, but I'll give you a good deal on it. ");
	addText("It'll cost you #b" .. itemPrice .. " mesos#k per item. ");
	addText("How many would you like to purchase?");
	quantity = askNumber(1, 100);

	local totalCost = itemPrice * quantity;
	addText("Are you sure you want to buy #r" .. quantity .. " #t" .. itemId .. "##k? ");
	addText("It'll cost you " .. itemPrice .. " mesos per #t" .. itemId .. "#, which will cost you #r" .. totalCost .. "#k mesos total.");
	choice = askYesNo();

	if choice == 1 then
		if not hasOpenSlotsFor(itemId, quantity) or getMesos() < totalCost then
			addText("Are you sure you have enough mesos? ");
			addText("Please check and see if your etc. or use inventory is full, or if you have at least #r" .. totalCost .. "#k mesos.");
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