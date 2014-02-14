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
-- Jane in Lith (1002100)

if isQuestCompleted(2013) then
	addText("It's you...! ");
	addText("Thanks to you, I was able to get much done. ");
	addText("Nowadays, I've been making a bunch of items. ");
	addText("If you need anything, just let me know.");
	sendNext();
	
	addText("What would you like to buy? \r\n");
	addText("#b#L0##t2000002# (price : 310 mesos) #l#k\r\n");
	addText("#b#L1##t2022003# (price : 1060 mesos) #l#k\r\n");
	addText("#b#L2##t2022000# (price : 1600 mesos) #l#k\r\n");
	addText("#b#L3##t2001000# (price : 3120 mesos) #l#k\r\n");
	choice = askChoice();

	item = nil;
	effect = nil;
	price = nil;
	if choice == 0 then
		item = 2000002;
		effect = "allows you to recover 300 HP";
		price = 310;
	elseif choice == 1 then
		item = 2022003;
		effect = "allows you to recover 1000 HP";
		price = 1060;
	elseif choice == 2 then
		item = 2022000;
		effect = "allows you to recover 800 MP";
		price = 1600;
	elseif choice == 3 then
		item = 2001000;
		effect = "allows you to recover 1000 HP and MP";
		price = 3120;
	end

	addText("You want #b#t" .. item .. "##k? ");
	addText("#t" .. item .. "# " .. effect .. ". ");
	addText("How many would you like to purchase?");
	qty = askNumber(0, 0, 100);

	finalCost = qty * price;

	addText("Will you purchase #r" .. qty .. "#k #b#t" .. item .. "#(s)#k? ");
	addText("#b#t" .. item .. "##k costs " .. price .. " per piece, so the total comes out to be " .. finalCost .. " mesos.");
	answer = askYesNo();	

	if answer == 1 then
		if hasOpenSlotsFor(item, qty) and getMesos() >= finalCost then
			giveItem(item, qty);
			giveMesos(-finalCost);
			addText("Thank you for coming! ");
			addText("Stuff here can always be made, so if you need something, please come again.");
			sendNext();
		else
			addText("Are you lacking mesos by any chance? ");
			addText("Please check to see if you have an empty slot available in the Etc window of your Item Inventory, and if you have at least #r" .. finalCost .. "#k mesos with you.");
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