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
-- Spiegelmann (every town one)

warrioritems = {
	-- Page 1
	{{1302004, 7},
	{1402006, 7},
	{1302009, 10},
	{1402007, 10},
	{1302010, 20},
	{1402003, 20},
	{1312006, 7},
	{1412004, 7},
	{1312007, 10},
	{1412005, 10},
	{1312008, 20},
	{1412003, 20}},

	-- Page 2
	{{1322015, 7},
	{1422008, 7},
	{1322016, 10},
	{1422007, 10},
	{1322017, 20},
	{1422005, 20},
	{1432003, 7},
	{1442003, 7},
	{1432005, 10},
	{1442009, 10},
	{1442005, 20},
	{1432004, 20}}
};

items = {
	-- Magician items
	{{1372001, 7},
	{1382018, 7},
	{1372012, 10},
	{1382019, 10},
	{1382001, 20},
	{1372007, 20}},

	-- Bowman items
	{{1452006, 7},
	{1452007, 10},
	{1452008, 20},
	{1462005, 7},
	{1462006, 10},
	{1462007, 20}},

	-- Thief items
	{{1472013, 7},
	{1472017, 10},
	{1472021, 20},
	{1332014, 7},
	{1332031, 10},
	{1332011, 10},
	{1332016, 20},
	{1332003, 20}},

	-- Pirate items
	{{1482005, 7},
	{1482006, 10},
	{1482007, 20},
	{1492005, 7},
	{1492006, 10},
	{1492007, 20}}
};

function giveWarriorReward(page)
	local rewardgroup = warrioritems[page];
	local rewardpair = rewardgroup[soption + 1];
	if getItemAmount(4001129) < rewardpair[2] or getOpenSlots(1) < 1 then
		addText("Check and see if you are either lacking #b#t4001129##k or if your Equipment inventory is full.");
		sendNext();
	else
		giveItem(4001129, rewardpair[2] * -1);
		giveItem(rewardpair[1], 1);
	end
end

function giveReward() -- For non-warriors
	local rewardgroup = items[boption - 2]; -- -2 would make option 3 as index 1, which is what we want
	local rewardpair = rewardgroup[soption + 1];
	if getItemAmount(4001129) < rewardpair[2] or getOpenSlots(1) < 1 then
		addText("Check and see if you are either lacking #b#t4001129##k or if your Equipment inventory is full.");
		sendNext();
	else
		giveItem(4001129, rewardpair[2] * -1);
		giveItem(rewardpair[1], 1);
	end
end

addText("What would you like to do? If you have never participated in the Monster Carnival, you'll need to know a thing or two about it before joining.\r\n");
addText("#b#L0# Go to the Monster Carnival Field.#l\r\n");
addText("#L1# Learn about the Monster Carnival.#l\r\n");
addText("#L2# Trade #t4001129#.#l");
option = askChoice();

if option == 0 then
	if getLevel() > 50 or getLevel() < 30 then
		addText("I'm sorry, but only the users within Level 30~50 may participate in Monster Carnival.");
		sendOK();
	else
		setPlayerVariable("mcout", getMap());
		setMap(980000000, "st00"); -- Need code to determine where player came from
	end
elseif option == 1 then
	while true do
		addText("What do you want to do?\r\n");
		addText("#b#L0# What's a Monster Carnival?#l\r\n");
		addText("#L1# General overview of the Monster Carnival#l\r\n");
		addText("#L2# Detailed instructions about the Monster Carnival#l\r\n");
		addText("#L3# Nothing, really. I've changed my mind.#l");
		foption = askChoice();
		
		if foption == 0 then
			addText("Haha! I'm Spiegelmann, the leader of this traveling carnival. I started the 1st ever #bMonster Carnival#k here, waiting for travelers like you to participate in this extravaganza!");
			sendNext();
			
			addText("What's a #bMonster Carnival#k? Hahaha! let's just say that it's an experience you will never forget! It's a #bbattle against other travelers like yourself!#k");
			sendBackNext();
			
			addText("I know that it is way too dangerous for you to fight one another using real weapons; nor would I suggest such a barbaric act. No my friend, what I offer is competition. The thrill of battle and the excitement of competing against people just as strong and motivated as yourself. I offer the premise that your party and the opposing party both #bsummon monsters, and defeat the monsters summoned by the opposing party. That's the essence of the Monster Carnival. Also, you can use Maple Coins earned during the Monster Carnival to obtain new items and weapons! #k");
			sendBackNext();
			
			addText("Of course, it's not as simple as that. There are different ways to prevent the other party from hunting monsters, and it's up to you to figure out how. What do you think? Interested in a little friendly (or not-so-friendly) competition?");
			sendBackNext();
		elseif foption == 1 then
			addText("#bMonster Carnival#k consists of 2 parties entering the battleground, and hunting the monsters summoned by the other party. It's a #bcombat quest that determines the victor by the amount of Carnival Points (CP) earned#k.");
			sendNext();
			
			addText("Once you enter the Carnival Field, the task is to #bearn CP by hunting monsters from the opposing party, and use those CP's to distract the opposing party from hunting monsters.#k.");
			sendBackNext();
			
			addText("There are three ways to distract the other party: #bSummon a Monster, Skill, and Protector#k. I'll give you a more in-depth look if you want to know more about 'Detailed Instructions'.");
			sendBackNext();
			
			addText("Please remember this, though. It's never a good idea to save up CP just for the sake of it. #bThe CP's you've used will also help determine the winner and the loser of the carnival#k.");
			sendBackNext();
		elseif foption == 2 then
			addText("Once you enter the Carnival Field, you'll see a Monster Carnival window appear. All you have to do is #bselect the ones you want to use, and press OK#k. Pretty easy, right?");
			sendNext();
			
			addText("Once you get used to the commands, try using #bthe Hotkeys TAB and F1 ~  F12#k. #bTAB toggles between Summoning Monsters/Skill/Protector,#k and, #bF1~ F12 allows you to directly enter one of the windows#k.");
			sendBackNext();
			
			addText("#bSummon a Monster#k calls a monster that attacks the opposing party, under your control. Use CP to bring out a Summoned Monster, and it'll appear in the same area, attacking the opposing party.");
			sendBackNext();
			
			addText("#bSkill#k is an option of using skills such as Darkness, Weakness, and others to prevent the opposing party from defeating the monsters. It requires a lot of CP, but it's well worth it. The only problem is that it doesn't last that long. Use this tactic wisely!");
			sendBackNext();
			
			addText("#bProtector#k is basically a summoned item that greatly boosts the abilities of the monster summoned by your party. Protector works as long as it's not demolished by the opposing party, so I suggest you summon a lot of monsters first, and then bring out the Protector.");
			sendBackNext();
			
			addText("Lastly, while you're in the Monster Carnival, #byou cannot use the recovery items/potions that you carry around with you.#k However, the monsters will drop those items every once in a while, and #bas soon as you pick it up, the item will activate immediately#k. That's why it's just as important to know WHEN to pick up those items.");
			sendBackNext();
		else
			break;
		end
	end
elseif option == 2 then
	addText("Remember, if you have Maple Coins, you can trade them in for items. Please make sure you have enough Maple Coins for the item you want. Select the item you'd like to trade for! \r\n");
	addText("#b#L0# #t1122007#(50 coins)#l\r\n");
	addText("#L1# #t2041211#(40 coins)#l\r\n");
	addText("#L2# Weapon for Warriors#l\r\n");
	addText("#L3# Weapon for Magicians#l\r\n");
	addText("#L4# Weapon for Bowmen#l\r\n");
	addText("#L5# Weapon for Thieves#l\r\n");
	addText("#L6# Weapon for Pirates#l");
	boption = askChoice();
	
	if boption == 0 or boption == 1 then -- Necklace garbage
		local requirement = 40;
		local reward = 2041211;
		if boption == 0 then
			requirement = 50;
			reward = 1122007;
		end
		if getItemAmount(4001129) < requirement or getOpenSlots(1) < 1 then
			addText("Check and see if you are either lacking #b#t4001129##k or if your Equipment inventory is full.");
			sendNext();
		else
			giveItem(4001129, requirement * -1);
			giveItem(reward, 1);
		end
	elseif boption == 2 then
		local page = 1;
		while true do
			if page == 1 then
				addText("Please make sure you have enough Maple Coins for the weapon you desire. Select the weapon you'd like to trade Maple Coins for. The selection I have is pretty good, if I do say so myself! \r\n");
				addText("#b#L0# #z1302004#(7 coins)#l\r\n");
				addText("#L1# #z1402006#(7 coins)#l\r\n");
				addText("#L2# #z1302009#(10 coins)#l\r\n");
				addText("#L3# #z1402007#(10 coins)#l\r\n");
				addText("#L4# #z1302010#(20 coins)#l\r\n");
				addText("#L5# #z1402003#(20 coins)#l\r\n");
				addText("#L6# #z1312006#(7 coins)#l\r\n");
				addText("#L7# #z1412004#(7 coins)#l\r\n");
				addText("#L8# #z1312007#(10 coins)#l\r\n");
				addText("#L9# #z1412005#(10 coins)#l\r\n");
				addText("#L10# #z1312008#(20 coins)#l\r\n");
				addText("#L11# #z1412003#(20 coins)#l\r\n");
				addText("#L12# Go to the Next Page(1/2)#l");
			else
				addText("#b#L0# #z1322015#(7 coins)#l\r\n");
				addText("#L1# #z1422008#(7 coins)#l\r\n");
				addText("#L2# #z1322016#(10coins)#l\r\n");
				addText("#L3# #z1422007#(10coins)#l\r\n");
				addText("#L4# #z1322017#(20 coins)#l\r\n");
				addText("#L5# #z1422005#(20 coins)#l\r\n");
				addText("#L6# #z1432003#(7 coins)#l\r\n");
				addText("#L7# #z1442003#(7 coins)#l\r\n");
				addText("#L8# #z1432005#(10coins)#l\r\n");
				addText("#L9# #z1442009#(10coins)#l\r\n");
				addText("#L10# #z1442005#(20 coins)#l\r\n");
				addText("#L11# #z1432004#(20 coins)#l\r\n");
				addText("#L12# Back to First Page(2/2)#l");
			end
			soption = askChoice();
			if soption ~= 12 then
				giveWarriorReward(page);
				break;
			end -- else, keep looping through the pages
			if page == 1 then
				page = 2
			else
				page = 1
			end
		end
	elseif boption == 3 then
		addText("Select the weapon you'd like to trade for. The weapons I have here are extremely appealing. See for yourself! \r\n");
		addText("#b#L0# #z1372001#(7 coins)#l\r\n");
		addText("#L1# #z1382018#(7 coins)#l\r\n");
		addText("#L2# #z1372012#(10coins)#l\r\n");
		addText("#L3# #z1382019#(10coins)#l\r\n");
		addText("#L4# #z1382001#(20 coins)#l\r\n");
		addText("#L5# #z1372007#(20 coins)#l");
		soption = askChoice();
		giveReward();
	elseif boption == 4 then
		addText("Select the weapon you'd like to trade for. The weapons I have here are highly appealing. See for yourself! \r\n");
		addText("#b#L0# #z1452006#(7 coins)#l\r\n");
		addText("#L1# #z1452007#(10 coins)#l\r\n");
		addText("#L2# #z1452008#(20 coins)#l\r\n");
		addText("#L3# #z1462005#(7 coins)#l\r\n");
		addText("#L4# #z1462006#(10 coins)#l\r\n");
		addText("#L5# #z1462007#(20 coins)#l");
		soption = askChoice();
		giveReward();
	elseif boption == 5 then
		addText("Select the weapon you'd like to trade for. The weapons I have here are of the highest quality. Select what appeals to you! \r\n");
		addText("#b#L0# #z1472013#(7 coins)#l\r\n");
		addText("#L1# #z1472017#(10coins)#l\r\n");
		addText("#L2# #z1472021#(20 coins)#l\r\n");
		addText("#L3# #z1332014#(7 coins)#l\r\n");
		addText("#L4# #z1332031#(10coins)#l\r\n");
		addText("#L5# #z1332011#(10coins)#l\r\n");
		addText("#L6# #z1332016#(20 coins)#l\r\n");
		addText("#L7# #z1332003#(20 coins)#l");
		soption = askChoice();
		giveReward();
	elseif boption == 6 then
		addText("Select the weapon you'd like to trade for. The weapons I have here are of the highest quality. Select what appeals to you! \r\n");
		addText("#b#L0# #z1482005#( 7coins)#l\r\n");
		addText("#L1# #z1482006#( 10coins)#l\r\n");
		addText("#L2# #z1482007#( 20coins)#l\r\n");
		addText("#L3# #z1492005#( 7coins)#l\r\n");
		addText("#L4# #z1492006#( 10coins)#l\r\n");
		addText("#L5# #z1492007#( 20coins)#l");
		soption = askChoice();
		giveReward();
	end
end