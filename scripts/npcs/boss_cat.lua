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

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/quizHelper.lua");

questions = {
	makeQuizQuestion("Which of these items does the Flaming Raccoon NOT drop?", 2, {"Raccoon Firewood ", "Solid Horn ", "Red Brick "}),
	makeQuizQuestion("Which one of these items has a mis-matched class or level description?", 1, {"Bamboo Spear - Warrior-only Weapon ", "Pico-Pico Hammer - One-handed Sword ", "Japanese Map - Level 50 equip. "}),
	makeQuizQuestion("What is the name of NPC that transfers from Kerning city to Mushroom Shrine?", 2, {" Pelican ", " Spinel ", " Transporter "}),
	makeQuizQuestion("Which of these items do the Thugs NOT drop?", 2, {"Thug A's Badge", "Thug B's Corset", "Thug C's Necklace"}),
	makeQuizQuestion("What is the name of statue in Mushroom Shrine?", 1, {"Mushroom Statue ", "Statue of Liberty ", "Maple Statue "}),
	makeQuizQuestion("What's the name of the vegetable store owner in Showa Town?", 3, {"Sami ", "Kami ", "Umi "}),
	makeQuizQuestion("Which of these items DO exist?", 3, {"Cloud Fox's Tooth ", "Ghost's Bouquet", "Nightfox's Tail "}),
	makeQuizQuestion("Which of these items DO NOT exist??", 3, {"Frozen Tuna ", "Sake ", "Fly Swatter "}),
	makeQuizQuestion("Which NPC is responsible for transporting travelers from Kerning City to Zipangu, and back?", 2, {"Peli ", "Spinel", "Poli "}),
	makeQuizQuestion("Which of the items sold at the Mushroom Shrine increases your attack power?", 1, {"Takoyaki ", "Yakisoba ", "Tempura "}),
	makeQuizQuestion("Which of these noodles are NOT being sold by Robo at the Mushroom Shrine?", 3, {"Kinoko Ramen (Pig Skull) ", "Kinoko Ramen (Salt) ", "Mushroom Miso Ramen"}),
	makeQuizQuestion("Which of these NPC's do NOT stand in front of Showa Movie Theater?", 3, {"Skye ", "Furano ", "Shinta "}),
};

if isQuestActive(8012) and getItemAmount(4031064) == 0 then
	addText("Did you get them all? ");
	addText("Are you going to try to answer all of my questions?");
	answer = askYesNo();

	if answer == answer_yes then
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
				if askQuizQuestion(questions, "Question no." .. i .. ": ") == answer_incorrect then
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