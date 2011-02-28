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
-- Francois - Item Creator (Ellinia)

dofile("scripts/lua_functions/itemProduction.lua");

wandids = {1372005, 1372006, 1372002, 1372004, 1372003, 1372001, 1372000, 1372007};
wandlimits = {8, 13, 18, 23, 28, 33, 38, 48};
wandjobs = {"all", "all", "all", "magician", "magician", "magician", "magician", "magician"};
wandreqs = {
	{4003001, 5, MESOS, 1000},
	{4003001, 10, 4000001, 50, MESOS, 3000},
	{4011001, 1, 4000009, 30, 4003000, 5, MESOS, 5000},
	{4011002, 2, 4003002, 1, 4003000, 10, MESOS, 12000},
	{4011002, 3, 4021002, 1, 4003000, 10, MESOS, 30000},
	{4021006, 5, 4011002, 3, 4011001, 1, 4003000, 15, MESOS, 60000},
	{4021006, 5, 4021005, 5, 4021007, 1, 4003003, 1, 4003000, 20, MESOS, 12000},
	{4011006, 4, 4021003, 3, 4021007, 2, 4021002, 1, 4003002, 1, 4003000, 30, MESOS, 200000}
};

staffids = {1382000, 1382003, 1382005, 1382004, 1382002, 1382001};
stafflimits = {10, 15, 15, 20, 25 ,45};
staffreqs = {
	{4003001, 5, MESOS, 2000},
	{4021005, 1, 4011001, 1, 4003000, 5, MESOS, 2000},
	{4021003, 1, 4011001, 1, 4003000, 5, MESOS, 2000},
	{4003001, 50, 4011001, 1, 4003000, 10, MESOS, 5000},
	{4021006, 2, 4021001, 1, 4011001, 1, 4003000, 15, MESOS, 12000},
	{4011001, 8, 4021006, 5, 4021001, 5, 4021005, 5, 4003000, 30, 4000010, 50, 4003003, 1, MESOS, 180000}
};

gloveids = {1082019, 1082020, 1082026, 1082051, 1082054, 1082062, 1082081, 1082086};
glovelimits = {15, 20, 25, 30, 35, 40, 50, 60};
glovestats = {"none", "none", "none", "none", "none", "none", "INT + 1", "INT + 1, LUK + 1"};
glovereqs = {
	{4000021, 15, MESOS, 500},
	{4000021, 30, 4011001, 1, MESOS, 300},
	{4000021, 50, 4011006, 2, MESOS, 500},
	{4000021, 60, 4021006, 1, 4021000, 2, MESOS, 800},
	{4000021, 70, 4011006, 1, 4011001, 3, 4021000, 2, MESOS, 500},
	{4000021, 80, 4021000, 3, 4021006, 3, 4003000, 30, MESOS, 800},
	{4021000, 3, 4011006, 2, 4000030, 55, 4003000, 40, MESOS, 1000},
	{4011007, 1, 4011001, 8, 4021007, 1, 4000030, 50, 4003000, 50, MESOS, 1500}
};

upgloveids = {1082021, 1082022, 1082027, 1082028, 1082052, 1082053, 1082055, 1082056, 1082063, 1082064, 1082082, 1082080, 1082087, 1082088};
upglovelimits = {20, 20, 25, 25, 30, 30, 35, 35, 40, 40, 50, 50, 60, 60};
upglovestats = {"INT + 1", "INT + 2", "INT + 1", "INT + 2", "INT + 1", "INT + 2", "INT + 1", "INT + 2", "INT + 2", "INT + 3", "INT + 2, MP + 15", "INT + 3, MP + 30", "INT + 2, LUK + 1, MP + 15", "INT + 3, LUK + 1, MP + 30"};
upglovereqs = {
	{1082020, 1, 4011001, 1, MESOS, 20000},
	{1082020, 1, 4021001, 2, MESOS, 25000},
	{1082026, 1, 4021000, 3, MESOS, 30000},
	{1082026, 1, 4021008, 1, MESOS, 40000},
	{1082051, 1, 4021005, 3, MESOS, 35000},
	{1082051, 1, 4021008, 1, MESOS, 40000},
	{1082054, 1, 4021005, 3, MESOS, 40000},
	{1082054, 1, 4021008, 1, MESOS, 45000},
	{1082062, 1, 4021002, 4, MESOS, 45000},
	{1082062, 1, 4021008, 2, MESOS, 50000},
	{1082081, 1, 4021002, 5, MESOS, 55000},
	{1082081, 1, 4021008, 3, MESOS, 60000},
	{1082086, 1, 4011004, 3, 4011006, 5, MESOS, 70000},
	{1082086, 1, 4021008, 2, 4011006, 3, MESOS, 80000}
};

hatids = {1002065,1002013};
hatlimits = {30, 30};
hatstats = {"INT + 1", "INT + 2"};
hatreqs = {
	{1002064, 1, 4011001, 3, MESOS, 40000},
	{1002064, 1, 4011006, 3, MESOS, 50000}
};

addText("Do you want to take a look at some items? Well... any ");
addText("thought of making one? I'm actually a wizard that was ");
addText("banished from the town because I casted an illegal magic. ");
addText("Because of that I've been hiding, doing some business ");
addText("here... well, that's not really the point. Do you want to do ");
addText("some business with me?");
yes = askYesNo();

if yes == 0 then
	addText("You don't trust my skills, I suppose... haha... you should ");
	addText("know that I used to be a great wizard. You still can't believe ");
	addText("my skills, huh... but just remember that I used to be the ");
	addText("great magician of old...");
	sendNext();
else
	addText("Alright ... it's for both of our own good, right? Choose what you want to do...\r\n");
	addText("#L0##b Make a wand#l\r\n");
	addText("#L1##b Make a staff#l\r\n");
	addText("#L2##b Make a glove#l\r\n");
	addText("#L3##b Upgrade a glove#l\r\n");
	addText("#L4##b Upgrade a hat#l\r\n");
	what = askChoice();

	if what == 0 then
		addText("If you gather up the materials for me, I'll make a wand for ");
		addText("you with my magical power. How... what kind of a wand do ");
		addText("you want to make?");
		for i = 1, 8 do
			addText("\r\n#L" .. i .. "##b #t" .. wandids[i] .. "##k(Level limit : " .. wandlimits[i] .. ", " .. wandjobs[i] .. ")#l");
		end
		what = askChoice();

		item = wandids[what];
		reqs = wandreqs[what];
		limit = wandlimits[what];
		addText("To make one #t" .. item .. "#, you'll need these items below. ");
		addText("The level limit for the item will be " .. limit .. " so please check and ");
		addText("see if you really need the item, first of all. Are you sure you ");
		addText("want to make one?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();

	elseif what == 1 then
		addText("If you gather up the materials for me, I'll make a staff for ");
		addText("you with my magical power. How... what kind of a wand do ");
		addText("you want to make?");
		for i = 1, 6 do
			addText("\r\n#L" .. i .. "##b #t" .. staffids[i] .. "##k(Level limit : " .. stafflimits[i] .. ", magician)#l");
		end
		what = askChoice();

		item = staffids[what];
		reqs = staffreqs[what];
		limit = stafflimits[what];
		addText("To make one #t" .. item .. "#, you'll need these items below. ");
		addText("The level limit for the item will be " .. limit .. " so please check and ");
		addText("see if you really need the item, first of all. ");
		addText("Are you sure you want to make one?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();

	elseif what == 2 then
		addText("If you gather up the materials for me, I'll make a glove for ");
		addText("you with my magical power. How... what kind of a wand do ");
		addText("you want to make?");
		for i = 1, 8 do
			addText("\r\n#L" .. i .. "##b #t" .. gloveids[i] .. "##k#(Level limit : " .. glovelimits[i] .. ", magician)#l");
		end
		what = askChoice();

		item = gloveids[what];
		reqs = glovereqs[what];
		limit = glovelimits[what];
		addText("To make one #t" .. item .. "#, you'll need these items below. ");
		addText("The level limit for the item will be " .. limit .. " so please check and ");
		addText("see if you really need the item, first of all. ");
		addText("Are you sure you want to make one?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();

	elseif what == 3 then
		addText("So you want to upgrade a glove? Be careful, though; ");
		addText("All the items that will be used for upgrading will be gone, ");
		addText("and if you use an item that has been #rupgraded#k with a scroll, ");
		addText("the effect will disappear when upgraded,");
		addText("so you may want to think about it before making your decision ...");
		sendNext();

		addText("Now .. which glove do you want to upgrade?");
		for i = 1, 14 do
			addText("\r\n#L" .. i .. "##b #t" .. upgloveids[i] .. "##k(Level limit : " .. upglovelimits[i] .. ", magician)#l");
		end
		what = askChoice();
		
		item = upgloveids[what];
		reqs = upglovereqs[what];
		limit = upglovelimits[what];
		stat = upglovestats[what];
		addText("To upgrade one #t" .. item .. "#, you'll need these items below.");
		addText("The level limit for the item is " .. limit .. ", ");
		if not(stat == "none") then
			addText("with the item option of #r" .. stat .. "#k attached to it, ");
		end
		addText("so please check and see if you really need it. ");
		addText("Oh, and one thing. Please make sure NOT to use an upgraded item as a material for the upgrade. ");
		addText("Now, are you sure you want to make this item?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();

	elseif what == 4 then
		addText("So you want to upgrade a hat ... Be careful, though; ");
		addText("All the items that will be used for upgrading will be gone, ");
		addText("and if you use an item that has been #rupgraded#k with a scroll, ");
		addText("the effect will disappear when upgraded,");
		addText("so you may want to think about it before making your decision ...");
		sendNext();

		addText("Alright, so which hat would you like to upgrade?");
		for i = 1, 2 do
			addText("\r\n#L" .. i .. "##b #t" .. hatids[i] .. "##k(Level limit : " .. hatlimits[i] .. ", wizard)#l");
		end
		what = askChoice();
		
		item = hatids[what];
		reqs = hatreqs[what];
		limit = hatlimits[what];
		stat = hatstats[what];
		addText("To upgrade one #t" .. item .. "#, you'll need these items below.");
		addText("The level limit for the item is " .. limit .. ", ");
		if not(stat == "none") then
			addText("with the item option of #r" .. stat .. "#k attached to it, ");
		end
		addText("so please check and see if you really need it. ");
		addText("Oh, and one thing. Please make sure NOT to use an upgraded item as a material for the upgrade. ");
		addText("Now, are you sure you want to make this item?\r\n\r\n");
		displayResources(reqs);
		yes = askYesNo();
	end
	if yes == 0 then
		addText("Really? You must be lacking materials. Try harder at ");
		addText("gathering them up around town. Fortunately it looks like the ");
		addText("monsters around the forest have various materials on their sleeves.");
		sendNext();
	else
		if (not hasResources(reqs)) or (not hasOpenSlotsFor(item, 1)) then
			addText("Please check and see if you have all the items you need, or if your equipment inventory is full or not.");
		else
			addText("Here, take #t" .. item .. "#. The more I see it, the more it looks perfect.");
			addText("Hahah, it's not a stretch to think that other magicians fear my skills ...");
			takeResources(reqs);
			giveItem(item, 1);
		end
		sendNext();
	end
end
