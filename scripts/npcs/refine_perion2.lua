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
-- Mr. Smith - Item Creator (Perion)

dofile("scripts/lua_functions/itemProduction.lua");

makeids = {1082003, 1082000, 1082004, 1082001, 1082007, 1082008, 1082023, 1082009, 1082059};
makelimits = {10, 15, 20, 25, 30, 35, 40, 50, 60};
makereqs = {
	{4000021, 15, 4011001, 1, MESOS, 1000},
	{4011001, 2, MESOS, 2000},
	{4000021, 40, 4011000, 2, MESOS, 5000},
	{4011001, 2, MESOS, 10000},
	{4011000, 3, 4003000, 15, MESOS, 20000},
	{4000021, 30, 4011001, 4, 4003000, 30, MESOS, 30000},
	{4000021, 50, 4011001, 5, 4003000, 40, MESOS, 40000},
	{4011001, 3, 4021007, 2, 4000030, 30, 4003000, 45, MESOS, 50000},
	{4011007, 1, 4011000, 8, 4011006, 2, 4000030, 50, 4003000, 50, MESOS, 70000}
};

upgradeids = {1082005, 1082006, 1082035, 1082036, 1082024, 1082025, 1082010, 1082011, 1082060, 1082061};
upgradelimits = {30, 30, 35, 35, 40, 40, 50, 50, 60, 60};
upgradereqs = {
	{1082007, 1, 4011001, 1, MESOS, 20000},
	{1082007, 1, 4011005, 2, MESOS, 25000},
	{1082008, 1, 4021006, 3, MESOS, 30000},
	{1082008, 1, 4021008, 1, MESOS, 40000},
	{1082023, 1, 4011003, 4, MESOS, 45000},
	{1082023, 1, 4021008, 2, MESOS, 50000},
	{1082009, 1, 4011002, 5, MESOS, 55000},
	{1082009, 1, 4011006, 4, MESOS, 60000},
	{1082059, 1, 4011002, 3, 4021005, 5, MESOS, 70000},
	{1082059, 1, 4021007, 2, 4021008, 2, MESOS, 80000}
};

materialids = {4003001, 4003001, 4003000};
materialamts = {1, 1, 15};
materialreqs = {
	{4000003, 10},
	{4000018, 5},
	{4011000, 1, 4011001, 1}
};

addText("I am Mr. Thunder's apprentice. He's gettin up there with age, and he isn't what he used to be...haha, oh crap, please don't tell him that I said that...Anyway...I can make various items specifically designed for the warriors, so what do you think? Wanna leave it up to me?");
yes = askYesNo();

if yes == 0 then
	addText("Sigh ... I'll definitely hear it from my boss if I don't make the norm today ... Oh well, that sucks.");
	sendNext();
else
	addText("Alright! The service fee will be reasonable so don't worry about it. What do you want to do?\r\n");
	addText("#b#L0# Make a glove#l\r\n");
	addText("#L1# Upgrade a glove#l\r\n");
	addText("#L2# Create materials#l");
	what = askChoice();
	
	if what == 0 then
		addText("I'm the best glove-maker in this town!! Now...what kind of a glove do you want to make?\r\n");
		addText("#L0##b #t1082003##k(level limit : 10, warrior)#l\r\n");
		addText("#L1##b #t1082000##k(level limit : 15, warrior)#l\r\n");
		addText("#L2##b #t1082004##k(level limit : 20, warrior)#l\r\n");
		addText("#L3##b #t1082001##k(level limit : 25, warrior)#l\r\n");
		addText("#L4##b #t1082007##k(level limit : 30, warrior)#l\r\n");
		addText("#L5##b #t1082008##k(level limit : 35, warrior)#l\r\n");
		addText("#L6##b #t1082023##k(level limit : 40, warrior)#l\r\n");
		addText("#L7##b #t1082009##k(level limit : 50, warrior)#l\r\n");
		addText("#L8##b #t1082059##k(level limit : 60, warrior)#l");
		what = askChoice() + 1;
		
		glove = makeids[what];
		reqs = makereqs[what];
		level = makelimits[what];
		addText("To make one #t" .. glove .. "#, I need the following items. The level limit is " .. level .. " and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
		displayResources(reqs);
		yes = askYesNo();
		
		if yes == 0 then
			addText("Lacking the materials? It's ok ... collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(glove, 1)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
			else
				addText("Here! take the #t" .. glove .. "#. Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				takeResources(reqs);
				giveItem(glove, 1);
			end
			sendNext();
		end
	elseif what == 1 then
		addText("So you want to upgrade the glove? Ok, then. A word of warning, though: All the items that will be used for upgrading will be gone, but if you use an item that has been #rupgraded#k with a scroll, the effect will disappear when upgraded. Please take that into consideration when making the decision, ok?");
		sendNext();
		
		addText("So~~ what kind of a glove do you want to upgrade and create?\r\n");
		addText("#L0##b #t1082005##k(level limit : 30, warrior)#l\r\n");
		addText("#L1##b #t1082006##k(level limit : 30, warrior)#l\r\n");
		addText("#L2##b #t1082035##k(level limit : 35, warrior)#l\r\n");
		addText("#L3##b #t1082036##k(level limit : 35, warrior)#l\r\n");
		addText("#L4##b #t1082024##k(level limit : 40, warrior)#l\r\n");
		addText("#L5##b #t1082025##k(level limit : 40, warrior)#l\r\n");
		addText("#L6##b #t1082010##k(level limit : 50, warrior)#l\r\n");
		addText("#L7##b #t1082011##k(level limit : 50, warrior)#l\r\n");
		addText("#L8##b #t1082060##k(level limit : 60, warrior)#l\r\n");
		addText("#L9##b #t1082061##k(level limit : 60, warrior)#l");
		what = askChoice();
		
		glove = upgradeids[what + 1];
		reqs = upgradereqs[what + 1];
		level = upgradelimits[what + 1];
		addText("To make one #t" .. glove .. "#, I need the following items. The level limit is " .. level .. " and please make sure you don't use an item that's been upgraded as a material for it. What do you think? Do you want one?\r\n");
		displayResources(reqs);
		yes = askYesNo();
		
		if yes == 0 then
			addText("Lacking the materials? It's ok ... collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if (not hasResources(reqs)) or (not hasOpenSlotsFor(glove, 1)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
			else
				addText("Here! take the #t" .. glove .. "#. Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				takeResources(reqs);
				giveItem(glove, 1);
			end
			sendNext();
		end
	elseif what == 2 then
		addText("So you want to make some materials, huh? Okay...what kind of materials do you want to make?\r\n");
		addText("#L0##bMake #t4003001# with #t4000003##k#l\r\n");
		addText("#L1##bMake #t4003001# with #t4000018##k#l\r\n");
		addText("#L2##bMake #t4003000#s#k#l");
		what = askChoice();
		
		material = materialids[what + 1];
		reqs = materialreqs[what + 1];
		amt = materialamts[what + 1];
		if what == 0 then
			addText("With #b10 #t4000003#es#k, I can make 1 #t4003001#(s). Be thankful, because this one's on me. What do you think? How many do you want?");
		elseif what == 1 then
			addText("With #b5 #t4000018#s#k, I can make 1 #t4003001#(s). Be thankful, because this one's on me. What do you think? How many do you want?");
		elseif what == 2 then
			addText("With #b1 #t4011001#(s) and #t4011000#(s) each#k, I can make 15 #t4003000#s. Be thankful, because this one's on me. What do you think? How many do you want?");
		end
		extra = askNumber(0, 0, 100);
		
		-- Output for this step is all messed up, thank Nexon
		if what == 0 then
			addText("You want to make #b#t4003001#(s)#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4000003#es#k then.");
		elseif what == 1 then
			addText("You want to make #b#t4003001#(s)#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4000018#s#k then.");
		elseif what == 2 then
			addText("You want to make #b#t4003000#s#k " .. extra .. " time(s)? I'll need  #r" .. reqs[2] * extra .. " #t4011001#(s) and #t4011000#(s) each#k then.");
		end
		yes = askYesNo();
		
		if 1 == 0 then
			addText("Lacking the materials? It's all good...collect them all and then come find me, alright? I'll be waiting...");
			sendNext();
		else
			if extra == 0 or (not hasResources(reqs)) or (not hasOpenSlotsFor(material, amt)) then
				addText("Check and see if you have everything you need and if your equipment inventory may be full or not.");
				sendNext();
			else
				takeResources(reqs, extra);
				giveItem(material, amt * extra);
				addText("Here! take " .. amt * extra  .. " #t" .. material .. "#(s). Don't you think I'm as good as Mr. Thunder? You'll be more than satisfied with what I made here.");
				sendNext();
			end
		end
	end
end
