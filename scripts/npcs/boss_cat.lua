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
-- Boss Kitty in Showa

if questions == nil then
	questions = {
		[1] = {
			["question"] = "Which of these items does the Flaming Raccoon NOT drop? #b\r\n#L0#Raccoon Firewood #l\r\n#L1#Solid Horn #l\r\n#L2#Red Brick #l",
			["answer"] = 1
		},
		[2] = {
			["question"] = "Which one of these items has a mis-matched class or level description? #b\r\n#L0#Bamboo Spear - Warrior-only Weapon #l\r\n#L1#Pico-Pico Hammer - One-handed Sword #l\r\n#L2#Japanese Map - Level 50 equip. #l",
			["answer"] = 0
		},
		[3] = {
			["question"] = "What is the name of NPC that transfers from Kerning city to Mushroom Shrine? #b\r\n#L0# Pelican #l\r\n#L1# Spinel #l\r\n#L2# Transporter #l",
			["answer"] = 1
		},
		[4] = {
			["question"] = "Which of these items do the Thugs NOT drop?#b\r\n#L0#Thug A's Badge#l\r\n#L1#Thug B's Corset#l\r\n#L2#Thug C's Necklace#l",
			["answer"] = 1
		},
		[5] = {
			["question"] = "What is the name of statue in Mushroom Shrine? #b\r\n#L0#Mushroom Statue #l\r\n#L1#Statue of Liberty #l\r\n#L2#Maple Statue #l",
			["answer"] = 0
		},
		[6] = {
			["question"] = "What's the name of the vegetable store owner in Showa Town? #b\r\n#L0#Sami #l\r\n#L1#Kami #l\r\n#L2#Umi #l",
			["answer"] = 2
		},
		[7] = {
			["question"] = "Which of these items DO exist? #b\r\n#L0#Cloud Fox's Tooth #l\r\n#L1#Ghost's Bouquet#l\r\n#L2#Nightfox's Tail #l",
			["answer"] = 2
		},
		[8] = {
			["question"] = "Which of these items DO NOT exist?? #b\r\n#L0#Frozen Tuna #l\r\n#L1#Sake #l\r\n#L2#Fly Swatter #l",
			["answer"] = 2
		},
		[9] = {
			["question"] = "Which NPC is responsible for transporting travelers from Kerning City to Zipangu, and back? #b\r\n#L0#Peli #l\r\n#L1#Spinel#l\r\n#L2#Poli #l",
			["answer"] = 1
		},
		[10] = {
			["question"] = "Which of the items sold at the Mushroom Shrine increases your attack power? #b\r\n#L0#Takoyaki #l\r\n#L1#Yakisoba #l\r\n#L2#Tempura #l",
			["answer"] = 0
		},
		[11] = {
			["question"] = "Which of these noodles are NOT being sold by Robo at the Mushroom Shrine? #b\r\n#L0#Kinoko Ramen (Pig Skull) #l\r\n#L1#Kinoko Ramen (Salt) #l\r\n#L2#Mushroom Miso Ramen#l",
			["answer"] = 2
		},
		[12] = {
			["question"] = "Which of these NPC's do NOT stand in front of Showa Movie Theater? #b\r\n#L0#Skye #l\r\n#L1#Furano #l\r\n#L2#Shinta #l",
			["answer"] = 2
		},
	};
end

if questionRecord == nil then
	questionRecord = {};
end

function question(id)
	local questionId = 0;
	repeat
		questionId = getRandomNumber(#questions);
	until questionRecord[questionId] == nil

	local q = questions[questionId];
	addText("Question no." .. id .. ": ");
	addText(q["question"]);
	local choice = askChoice();

	if choice == q["answer"] then
		questionRecord[questionId] = true
		return true;
	end
	return false;
end

if isQuestActive(8012) and getItemAmount(4031064) == 0 then
	addText("Did you get them all? ");
	addText("Are you going to try to answer all of my questions?");
	answer = askYesNo();
	if answer == 1 then
		if not giveItem(2020001, -300) then
			addText("Hey, are you sure you brought the 300 Fried Chickens I asked for? ");
			addText("Check again and see if you brought enough.");
			sendNext();
		else
			addText("Good job! ");
			addText("Now hold on a sec...");
			addText("Hey look! ");
			addText("I got some food here! ");
			addText("Help yourselves. ");
			addText("Okay, now it's time for me to ask you some questions. ");
			addText("I'm sure you're aware of this, but remember, if you're wrong, it's over. ");
			addText("It's all or nothing!");
			sendNext();

			for i = 1, 5 do
				if not question(i) then
					addText("Hmmm...all humans make mistakes anyway! ");
					addText("If you want to take another crack at it, then bring me 300 Fried Chickens.");
					sendNext();
					return;
				end
			end

			addText("Dang, you answered all the questions right! ");
			addText("I may not like humans in general, but I HATE breaking a promise, so, as promised, here's the Orange Marble. ");
			addText("You earned it!");
			sendNext();

			if not giveItem(4031064, 1) then
				addText("Your Etc. inventory is FULL!?! ");
				addText("You need to make room there if you want the Orange Marble.");
				sendNext();			
			else
				addText("Our business is concluded, thank you very much! ");
				addText("You can leave now!");
				sendNext();
			end
		end
	else
		addText("You don't have the courage to face these questions. ");
		addText("I knew it...out of my sight!");
		sendNext();
	end
elseif isQuestCompleted(8012) or getItemAmount(4031064) > 0 then
	addText("Meeeoooowww!");
	sendNext();
else
	addText("Hahaha...you want to try to answer my diabolically hard questions? ");
	addText("Well, they aren't free--but the prize is worth it!");
	sendNext();
end