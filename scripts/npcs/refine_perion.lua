--[[
Copyright (C) 2008-2011 Vana Development Team

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

dofile("scripts/lua_functions/itemProduction.lua");

mineralids = {4011000, 4011001, 4011002, 4011003, 4011004, 4011005, 4011006};
mineralreqs = {
	{4010000, 10, MESOS, 300},
	{4010001, 10, MESOS, 300},
	{4010002, 10, MESOS, 300},
	{4010003, 10, MESOS, 500},
	{4010004, 10, MESOS, 500},
	{4010005, 10, MESOS, 500},
	{4010006, 10, MESOS, 800}
};

jewelids = {4021000, 4021001, 4021002, 4021003, 4021004, 4021005, 4021006, 4021007, 4021008};
jewelreqs = {
	{4020000, 10, MESOS, 500},
	{4020001, 10, MESOS, 500},
	{4020002, 10, MESOS, 500},
	{4020003, 10, MESOS, 500},
	{4020004, 10, MESOS, 500},
	{4020005, 10, MESOS, 500},
	{4020006, 10, MESOS, 500},
	{4020007, 10, MESOS, 1000},
	{4020008, 10, MESOS, 3000}
};

helmetids = {1002042,1002041,1002002,1002044,1002003,1002040,1002007,1002052,1002011,1002058,1002009,1002056,1002087,1002088,1002049,1002050,1002047,1002048,1002099,1002098,1002085,1002028,1002022,1002101};
helmetreqs = {
	{1002001, 1, 4011002, 1, MESOS, 500},
	{1002001, 1, 4021006, 1, MESOS, 300},
	{1002043, 1, 4011001, 1, MESOS, 500},
	{1002043, 1, 4011002, 1, MESOS, 800},
	{1002039, 1, 4011001, 1, MESOS, 500},
	{1002039, 1, 4011002, 1, MESOS, 800},
	{1002051, 1, 4011001, 2, MESOS, 1000},
	{1002051, 1, 4011002, 2, MESOS, 1500},
	{1002059, 1, 4011001, 3, MESOS, 1500},
	{1002059, 1, 4011002, 3, MESOS, 2000},
	{1002055, 1, 4011001, 3, MESOS, 1500},
	{1002055, 1, 4011002, 3, MESOS, 2000},
	{1002027, 1, 4011002, 4, MESOS, 2000},
	{1002027, 1, 4011006, 4, MESOS, 4000},
	{1002005, 1, 4011006, 5, MESOS, 4000},
	{1002005, 1, 4011005, 5, MESOS, 5000},
	{1002004, 1, 4021000, 3, MESOS, 8000},
	{1002004, 1, 4021005, 3, MESOS, 10000},
	{1002021, 1, 4011002, 5, MESOS, 12000},
	{1002021, 1, 4011006, 6, MESOS, 15000},
	{1002086, 1, 4011002, 5, MESOS, 20000},
	{1002086, 1, 4011004, 4, MESOS, 25000},
	{1002100, 1, 4011007, 1, 4011001, 7, MESOS, 30000},
	{1002100, 1, 4011007, 1, 4011002, 7, MESOS, 30000}
};
helmetstats = {"none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "none", "HP + 10", "DEX + 1", "STR + 1", "STR + 2", "STR + 1", "STR + 2", "DEX + 1, MP + 30", "STR + 1, MP + 30"};

shieldids = {1092013, 1092014, 1092010, 1092011};
shieldreqs = {
	{1092012, 1, 4011002, 10, MESOS, 100000},
	{1092012, 1, 4011003, 10, MESOS, 100000},
	{1092009, 1, 4011007, 1, 4011004, 15, MESOS, 120000},
	{1092009, 1, 4011007, 1, 4011003, 15, MESOS, 120000}
};
shieldstats = {"STR + 2", "DEX + 2", "DEX + 2", "STR + 2"};

addText("Wait, do you have the ore of either a jewel or mineral? ");
addText("With a little service fee, I can turn them into materials ");
addText("needed to create weapons or shields. Not only that, I can ");
addText("also upgrade an item with it to make an even better item. ");
addText("What do you think? Do you want to do it?");
yes = askYesNo();

if yes == 0 then
	addText("Really? Sorry to hear that. If you don't need it, then oh well ... ");
	addText("if you happen to collect a lot of ores in the future, please find ");
	addText("me. I'll make something only I can make.");
	sendNext();
else
	addText("Alright, with the ore and a little service fee, I'll refine it so you ");
	addText("can you use it. Check and see if your etc, storage has any room. ");
	addText("Now ... what would you like me to do?\r\n");
	addText("#L0##b Refine the raw ore of a mineral#l\r\n");
	addText("#L1##b Refine a jewel ore#l\r\n");
	addText("#L2##b Upgrade a helmet#l\r\n");
	addText("#L3##b Upgrade a shield#l\r\n");
	what = askChoice();
	
	if what == 0 then
		addText("Which mineral do you want to make?\r\n");
		addText("#L1##b #t4011000##k#l\r\n");
		addText("#L2##b #t4011001##k#l\r\n");
		addText("#L3##b #t4011002##k#l\r\n");
		addText("#L4##b #t4011003##k#l\r\n");
		addText("#L5##b #t4011004##k#l\r\n");
		addText("#L6##b #t4011005##k#l\r\n");
		addText("#L7##b #t4011006##k#l");
		what = askChoice();
		
		mineral = mineralids[what];
		reqs = mineralreqs[what];
		addText("To make a(n) #t" .. mineral .. "#, I need the following materials. How many would you like to make?\r\n\r\n");
		displayResources(reqs);
		amount = askNumber(0, 0, 100);

		addText("To make #b" .. amount .. " #t" .. reqs[1] .. "#(s)#k, I need the following materials. Are you sure you want to make these?\r\n\r\n");
		displayResources(reqs, amount);
		yes = askYesNo();
		
		if yes == 0 then
			addText("We have all kinds of items so don't panic, and choose the one you want to buy...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(mineral, amount)) or amount == 0 then
				addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
			else
				addText("Hey! Here, take " .. amount .. " #t" .. mineral .. "#(s). This came out even finer than I though ... a finely refined item like this, I don't think you'll see it anywhere else!! Please come again~");
				takeResources(reqs, amount);
				giveItem(mineral, amount);
			end
			sendNext();
		end
	elseif what == 1 then
		addText("Which jewel do you want to refine?\r\n");
		addText("#L1##b #t4021000##k#l\r\n");
		addText("#L2##b #t4021001##k#l\r\n");
		addText("#L3##b #t4021002##k#l\r\n");
		addText("#L4##b #t4021003##k#l\r\n");
		addText("#L5##b #t4021004##k#l\r\n");
		addText("#L6##b #t4021005##k#l\r\n");
		addText("#L7##b #t4021006##k#l\r\n");
		addText("#L8##b #t4021007##k#l\r\n");
		addText("#L9##b #t4021008##k#l");
		what = askChoice();
		
		jewel = jewelids[what];
		reqs = jewelreqs[what];
		addText("To make a(n) #t" .. jewel .. "#, I need the following materials. How many would you like to make?\r\n\r\n");
		displayResources(reqs);
		amount = askNumber(0, 0, 100);

		addText("To make #b" .. amount .. " #t" .. reqs[1] .. "#(s)#k, I need the following materials. Are you sure you want to make these?\r\n\r\n");
		displayResources(reqs, amount);
		yes = askYesNo();
		
		if yes == 0 then
			addText("We have all kinds of items so don't panic, and choose the one you want to buy...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(jewel, amount)) or amount == 0 then
				addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
			else
				addText("Hey! Here, take " .. amount .. " #t" .. jewel .. "#(s). This came out even finer than I though ... a finely refined item like this, I don't think you'll see it anywhere else!! Please come again~");
				takeResources(reqs, amount);
				giveItem(jewel, amount);
			end
			sendNext();
		end
	elseif what == 2 then
		addText("So you want to upgrade the helmet? Ok, then. A word of ");
		addText("warning, though: All the items that will be used for ");
		addText("upgrading will be gone, and if you use an item that has ");
		addText("been #rupgraded#k with a scroll before, the effect will not go in ");
		addText("when upgraded. Please take that info consideration when ");
		addText("making the decision, ok?");
		sendNext();

		addText("So~~ what kind of a helmet do you want to upgrade and create?\r\n");
		addText("#L1##b #t1002042##k(level limit: 15, all)#l\r\n");
		addText("#L2##b #t1002041##k(level limit: 15, all)#l\r\n");
		addText("#L3##b #t1002002##k(level limit: 10, warrior)#l\r\n");
		addText("#L4##b #t1002044##k(level limit: 10, warrior)#l\r\n");
		addText("#L5##b #t1002003##k(level limit: 12, warrior)#l\r\n");
		addText("#L6##b #t1002040##k(level limit: 12, warrior)#l\r\n");
		addText("#L7##b #t1002007##k(level limit: 15, warrior)#l\r\n");
		addText("#L8##b #t1002052##k(level limit: 15, warrior)#l\r\n");
		addText("#L9##b #t1002011##k(level limit: 20, warrior)#l\r\n");
		addText("#L10##b #t1002058##k(level limit: 20, warrior)#l\r\n");
		addText("#L11##b #t1002009##k(level limit: 20, warrior)#l\r\n");
		addText("#L12##b #t1002056##k(level limit: 20, warrior)#l\r\n");
		addText("#L13##b #t1002087##k(level limit: 22, warrior)#l\r\n");
		addText("#L14##b #t1002088##k(level limit: 22, warrior)#l\r\n");
		addText("#L15##b #t1002049##k(level limit: 25, warrior)#l\r\n");
		addText("#L16##b #t1002050##k(level limit: 25, warrior)#l\r\n");
		addText("#L17##b #t1002047##k(level limit: 35, warrior)#l\r\n");
		addText("#L18##b #t1002048##k(level limit: 35, warrior)#l\r\n");
		addText("#L19##b #t1002099##k(level limit: 40, warrior)#l\r\n");
		addText("#L20##b #t1002098##k(level limit: 40, warrior)#l\r\n");
		addText("#L21##b #t1002085##k(level limit: 50, warrior)#l\r\n");
		addText("#L22##b #t1002028##k(level limit: 50, warrior)#l\r\n");
		addText("#L23##b #t1002022##k(level limit: 55, warrior)#l\r\n");
		addText("#L24##b #t1002101##k(level limit: 55, warrior)#l");
		what = askChoice();
		
		helm = helmetids[what];
		reqs = helmetreqs[what];
		addText("To make one #t" .. helm .. "#, I need the following materials.");
		if helmetstats[what] ~= "none" then 
			addText("This item has an option of " .. helmetstats[what] ..". ");
		end
		addText("Make sure you don't use an item that's been upgraded as a material for it.");
		addText("What do you think? Do you want one?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();
		
		if yes == 0 then
			addText("Really? Sorry to hear that. Come back when you need me.");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(helm, 1)) then
				addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
			else
				addText("Hey! Here, take #t" .. helm .. "#. I'm good ... a finely refined item like this, have you seen it anywhere else?? Please come again~");
				takeResources(reqs);
				giveItem(helm, 1);
			end
			sendNext();
		end
	elseif what == 3 then
		addText("So you want to upgrade the shield? Ok, then. A word of ");
		addText("warning, though: All the items that will be used for ");
		addText("upgrading will be gone, and if you use an item that has ");
		addText("been #rupgraded#k with a scroll before, the effect will not go in ");
		addText("when upgraded. Please take that info consideration when ");
		addText("making the decision, ok?");
		sendNext();

		addText("So~~ what kind of a shield do you want to upgrade and create?\r\n");
		addText("#L1##b #t1092013##k(level limit: 40, warrior)#l\r\n");
		addText("#L2##b #t1092014##k(level limit: 40, warrior)#l\r\n");
		addText("#L3##b #t1092010##k(level limit: 60, warrior)#l\r\n");
		addText("#L4##b #t1092011##k(level limit: 60, warrior)#l\r\n");
		what = askChoice();
		
		shield = shieldids[what];
		reqs = shieldreqs[what];
		addText("To make one #t" .. shield .. "#, I need the following materials. ");
		if shieldstats[what] ~= "none" then 
			addText("This item has an option of " .. shieldstats[what] ..". ");
		end
		addText("Make sure you don't use an item that's been upgraded as a material for it. ");
		addText("What do you think? Do you want one?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();
		
		if yes == 0 then
			addText("Really? Sorry to hear that. Come back when you need me.");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(shield, 1)) then
				addText("Please double-check you have all the materials you need and if you etc. inventory may be full or not.");
			else
				addText("Hey! Here, take #t" .. shield .. "#. I'm good ... a finely refined item like this, have you seen it anywhere else?? Please come again~");
				takeResources(reqs);
				giveItem(shield, 1);
			end
			sendNext();
		end
	end
end
