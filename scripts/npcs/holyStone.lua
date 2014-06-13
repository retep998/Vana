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
-- Holy Stone for third job advancements

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/quizHelper.lua");
dofile("scripts/utils/miscHelper.lua");

questions = {
	makeQuizQuestion("Which of these NPCs will you see FIRST at MapleStory...?", 1, {" " .. npcRef(2000), " " .. npcRef(1010100), " " .. npcRef(2102), " " .. npcRef(2001), " " .. npcRef(2101)}),
	makeQuizQuestion("Which of these EXP points is required to go from Lv. 1 to 2...?", 2, {" 10", " 15", " 20", " 25", " 30"}),
	makeQuizQuestion("Which of these NPCs will you NOT see at El Nath of Ossyria...?", 3, {" " .. npcRef(2020000), " " .. npcRef(2020003), " " .. npcRef(2012010), " " .. npcRef(2020006), " " .. npcRef(2020007)}),
	makeQuizQuestion("Which of these jobs will you NOT get after the 2nd job advancement...?", 5, {" Page", " Assassin", " Bandit", " Cleric", "Buccaneer"}),
	makeQuizQuestion("Which of these quests can be repeated...?", 4, {" Maya and the Weird Medicine", " Alex the Runaway Kid", " Pia and the Blue Mushroom", " Arwen and the Glass Shoes", " Alpha Platoon's Network of Communication"}),
	makeQuizQuestion("Which of these pairings of sickness/results resulted from a monster's attack does NOT match...?", 3, {" State of darkness - decrease in accuracy", " State of being cursed - decrease in EXP earned", " State of weakness - decrease in speed", " State of being sealed up - unable to use skills", " State of being poisoned - slow decrease in HP"}),
	makeQuizQuestion("Which of these NPC's will you NOT see at Orbis of Ossyria...?", 2, {" Staff Sergeant Charlie", " Sophia", " Neve", " Nuri the Fairy", " Edel the Fairy"}),
	makeQuizQuestion("Which of these quests has the highest required level to start...?", 4, {" Manji's Old Gladius", " Luke the Security Man's Wish to Travel", " Looking for the Book of Ancient", " Alcaster and the Black Crystal", " Alpha Platoon's Network of Communication"}),
	makeQuizQuestion("Which of these NPC's have NOTHING to do with refining, upgrading, and making items...?", 3, {" Neve", " Mr. Thunder", " Shane", " Francois", " Vogen"}),
	makeQuizQuestion("In MapleStory, which of these pairings of potion/results matches?", 5, {" Orange Potion - Recover HP 200", " Ice Cream Pop - Recover MP 2000", " Lemon - Recover MP 100", " Fried Chicken - Recover HP 300", " Pizza - Recover HP 400"}),
	makeQuizQuestion("Which of the pairings of the monster/leftover is correctly matched...?", 4, {" Fire Boar - Fire Boar's nose", " Cold Eye - Cold Eye's Eye", " Pig - Pig's ear", " Stirge - Stirge Wing", " Zombie Mushroom - Zombie Mushroom's top"}),
	makeQuizQuestion("Which of these NPC's will you NOT see at Perion of Victoria Island?", 2, {" Arturo", " Francois", " Manji", " Mr. Thunder", " Sophia"}),
	makeQuizQuestion("Which of these NPC's is the father of Alex the Runaway Kid, whom you'll see at Kerning City?", 5, {" Cloy", " Vicious", " Lucas", " John", " Chief Stan"}),
	makeQuizQuestion("Which of these items will you receive from the NPC after collecting 30 Dark Marbles during the test for the 2nd job advancement?", 1, {" The Proof of a Hero", " Hero's Necklace", " Hero's Pendant", " Hero's Medal", " Hero's Sign"}),
	makeQuizQuestion("Which of these pairings of job/required stat matches for the 1st job advancement?", 3, {" Warrior - STR 30+ ", " Magician - INT 25+ ", " Bowman - DEX 25+ ", " Thief - DEX 20+ ", " Thief - LUK 20+ "}),
	makeQuizQuestion("Which of these NPC's will you NOT see at Ellinia of Victoria Island...?", 5, {" Shane", " Francois", " Grendel the Really Old", " Arwen the Fairy", " Roel"}),
	makeQuizQuestion("Which of these monsters will you NOT be facing Ossyria...?", 2, {" White Fang", " Croco", " Yeti", " Lycanthrope", " Luster Pixie"}),
	makeQuizQuestion("Which of these monsters have the highest level...?", 4, {" Octopus", " Ribbon Pig", " Green Mushroom", " Axe Stump", " Bubbling"}),
	makeQuizQuestion("In MapleStory, Which of these pairings of potions/results does not match...?", 2, {" Holy Water - Recover from the state of being cursed or sealed up", " Sunrise Dew - Recover MP 3000", " Hamburger - Recover HP 400", " Salad - Recover MP 200", " Blue Potion - Recover MP 100"}),
	makeQuizQuestion("Which of these NPC's have NOTHING to do with pets...?", 4, {" Cloy", " Mar the Fairy", " Trainer Frod", " Vicious", " Doofus"}),
	makeQuizQuestion("Which NPC is not a member of the Alpha Platoon scattered over the continent of Ossyria?", 1, {" Staff Sergeant Peter", " Staff Sergeant Charlie", " Master Sergeant Fox", " Corporal Easy", " Sergeant Bravo"}),
	makeQuizQuestion("Which of the following items is not needed to awaken the Old Gladius from Manji in Perion?", 4, {" Piece of Ice", " Star Rock", " Flaming Feather", " Fairy Wing", " Ancient Scroll"}),
	makeQuizQuestion("Which of the following monsters can fly?", 5, {" Hector", " Nependeath", " Iron Hog", " Tortie", " Malady"}),
	makeQuizQuestion("Which of these NPC's will you NOT see at Henesys of Victoria Island...?", 2, {" Maya", " Teo", " Rina", " Athena Pierce", " Pia"}),
	makeQuizQuestion("Which of these monsters will you NOT see at Maple Island...?", 4, {" Blue Snail", " Orange Mushroom", " Red Snail", " Pig", " Shroom"}),
	makeQuizQuestion("Which of these items did Maya ask for the cure of her sickness...?", 5, {" Incredible medicine", " Nasty medicine", " The all-curing medicine", " Chinese medicine", " Weird Medicine"}),
	makeQuizQuestion("Which of these towns is NOT part of Victoria Island...?", 2, {" Sleepywood", " Amherst", " Perion", " Kerning City", " Ellinia"}),
	makeQuizQuestion("Which of these monsters will you NOT see at the ant tunnel and the center dungeon of Victoria Island...?", 5, {" Horny Mushroom", " Evil Eye", " Drake", " Zombie Mushroom", " Sentinel"}),
};

questState = getPlayerVariable("third_job_advancement", type_int);
if questState == nil or questState ~= 3 then
	-- TODO FIXME: Text?
else
	addText("... ... ...\r\n");
	addText("If you want to test out your wisdom, then you'll have to offer a " .. blue(itemRef(4005004)) .. " as a sacrifice ...\r\n");
	addText("Are you ready to offer a " .. itemRef(4005004) .. " and answer a set of questions from me?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Come back when you're ready.");
		sendNext();
	else
		if getItemAmount(4031058) > 0 then
			addText("You already have " .. blue(itemRef(4031058)) .. " with you...\r\n");
			addText("Take the necklace and go back...");
			sendNext();
		else
			if getItemAmount(4005004) == 0 then
				addText("If you want me to test your wisdom, you will have to provide a " .. blue(itemRef(4005004)) .. " as a sacrifice.");
				sendNext();
			else
				giveItem(4005004, -1);

				addText("Alright ... I'll be testing your wisdom here. ");
				addText("Answer all the questions correctly, and you will pass the test. ");
				addText("But, if you are wrong just once, then you'll have to start over again... ");
				addText("Okay, here we go.");
				sendNext();

				for i = 1, 5 do
					if askQuizQuestion(questions, "Here's the " .. i .. getOrdinal(i) .. " question. ") == answer_incorrect then
						addText("Wrong... Start again...");
						sendNext();
						return;
					end
				end

				addText("Alright. All your answers have been proven as the truth...\r\n");
				addText("Your wisdom has been proven.\r\n");
				addText("Take this necklace and go back...");
				sendNext();

				if not giveItem(4031058, 1)  then
					-- TODO FIXME text?
				end
			end
		end
	end
end