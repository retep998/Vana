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
-- Robin

dofile("scripts/lua_functions/npcHelper.lua");

iterationBreak = 1;

choices = {
	makeChoiceHandler("How do I move?", function()
		addText("Alright this is how you move. ");
		addText("Use " .. blue("left, right arrow") .. " to move around the flatland and slanted roads, and press " .. blue("Alt") .. " to jump. ");
		addText("A select number of shoes improve your speed and jumping abilities.");
		sendNext();
	end),
	makeChoiceHandler("How do I take down the monsters?", function()
		addText("Here's how to take down a monster. ");
		addText("Every monster possesses an HP of its own and you'll take them down by attacking with either a weapon or through spells. ");
		addText("Of course, the stronger they are, the harder it is to take them down.");
		sendNext();

		addText("In order to attack the monsters you'll need to be equipped with a weapon. ");
		addText("When equipped, press " .. blue("Ctrl") .. " to use the weapon. ");
		addText("With the right timing, you'll be able to easily take down the monsters.");
		sendBackNext();

		addText("Once you make the job advancement, you'll acquire different kinds of skills, and you can assign them to HotKeys for easier access. ");
		addText("If it's an attacking skill, you don't need to press Ctrl to attack; just press the button assigned as a HotKey.");
		sendBackNext();
	end),
	makeChoiceHandler("How can I pick up an item?", function()
		addText("This is how you gather up an item. ");
		addText("Once you take down a monster, an item will be dropped to the ground. ");
		addText("When that happens, stand in front of the item and press " .. blue("Z") .. " or " .. blue("0 on the NumPad") .. " to acquire the item.");
		sendNext();

		addText("Remember, though, that if your item inventory is full, you won't be able to acquire more. ");
		addText("So if you have an item you don't need, sell it so you can make something out of it. ");
		addText("The inventory may expand once you make the job advancement.");
		sendBackNext();
	end),
	makeChoiceHandler("What happens when I die?", function()
		addText("Curious to find out what happens when you die? ");
		addText("You'll become a ghost when your HP reaches 0. ");
		addText("There will be a tombstone in that place and you won't be able to move, although you still will be able to chat.");
		sendNext();

		addText("There isn't much to lose when you die if you are just a beginner. ");
		addText("Once you have a job, however, it's a different story. ");
		addText("You'll lose a portion of your EXP when you die, so make sure you avoid danger and death at all cost.");
		sendBackNext();
	end),
	makeChoiceHandler("When can I choose a job?", function()
		addText("When do you get to choose your job? ");
		addText("Hahaha, take it easy, my friend. ");
		addText("Each job has a requirement set for you to meet. ");
		addText("Normally a level between 8 and 10 will do, so work hard.");
		sendNext();

		addText("Level isn't the only thing that determines the advancement, though. ");
		addText("You also need to boost up the levels of a particular ability based on the occupation. ");
		addText("For example, to be a warrior, your STR has to be over 35, and so forth, you know what I'm saying? ");
		addText("Make sure you boost up the abilities that has direct implications to your job.");
		sendBackNext();
	end),
	makeChoiceHandler("Tell me more about this island!", function()
		addText("Want to know about this island? ");
		addText("It's called Maple Island and it floats in the air. ");
		addText("It's been floating in the sky for a while so the nasty monsters aren't really around. ");
		addText("It's a very peaceful island, perfect for beginners!");
		sendNext();

		addText("But, if you want to be a powerful player, better not think about staying here for too long. ");
		addText("You won't be able to get a job anyway. ");
		addText("Underneath this island lies an enormous island called Victoria Island. ");
		addText("That place is so much bigger than here, it's not even funny.");
		sendBackNext();

		addText("How do you get to Victoria Island? ");
		addText("On the east of this island there's an harbor called " .. mapRef(60000) .. ". ");
		addText("There, you'll find a ship that flies in the air. ");
		addText("In front of the ship stands the captain. ");
		addText("Ask him about it.");
		sendBackNext();

		addText("Oh yeah! ");
		addText("One last piece of information before I go. ");
		addText("If you are not sure where you are, always press " .. blue("W") .. ". ");
		addText("The world map will pop up with the locator showing where you stand. ");
		addText("You won't have to worry about getting lost with that.");
		sendBackNext();
	end),
	makeChoiceHandler(" What should I do to become a Warrior?", function()
		addText("You want to become a " .. blue("warrior") .. "? ");
		addText("Hmmm, then I suggest you head over to Victoria Island. ");
		addText("Head over to a warrior-town called " .. red("Perion") .. " and see " .. blue("Dances with Balrog") .. ". ");
		addText("He'll teach you all about becoming a true warrior. ");
		addText("Ohh, and one VERY important thing: You'll need to be at least level 10 in order to become a warrior!!");
		sendNext();
	end),
	makeChoiceHandler(" What should I do to become a Bowman?", function()
		addText("You want to become a " .. blue("bowman") .. "? ");
		addText("You'll need to go to Victoria Island to make the job advancement. ");
		addText("Head over to a bowman-town called " .. red("Henesys") .. " and talk to the beautiful " .. blue("Athene Pierce") .. " and learn the in's and out's of being a bowman. ");
		addText("Ohh, and one VERY important thing: You'll need to be at least level 10 in order to become a bowman!!");
		sendNext();
	end),
	makeChoiceHandler(" What should I do to become a Thief?", function()
		addText("You want to become a " .. blue("thief") .. "? ");
		addText("In order to become one, you'll have to head over to Victoria Island. ");
		addText("Head over to a thief-town called " .. red("Kerning City") .. ", and on the shadier side of town, you'll see a thief's hideaway. ");
		addText("There, you'll meet " .. blue("Dark Lord") .. " who'll teach you everything about being a thief. ");
		addText("Ohh, and one VERY important thing: You'll need to be at least level 10 in order to become a thief!!");
		sendNext();
	end),
	makeChoiceHandler(" What should I do to become a Magician?", function()
		addText("You want to become a " .. blue("magician") .. "? ");
		addText("For you to do that, you'll have to head over to Victoria Island. ");
		addText("Head over to a magician-town called " .. red("Ellinia") .. ", and at the very top lies the Magic Library. ");
		addText("Inside, you'll meet the head of all wizards, " .. blue("Grendel the Really Old") .. ", who'll teach you everything about becoming a wizard. ");
		sendNext();

		addText("Oh by the way, unlike other jobs, to become magician you only need to be at level 8. ");
		addText("What comes with making the job advancement early also comes with the fact that it takes a lot to become a true powerful mage. ");
		addText("Think long and carefully before choosing your path.");
		sendBackNext();
	end),
	makeChoiceHandler(" What should I do to become a Pirates?", function()
		addText("Do you wish to become a " .. blue("Pirate") .. "? ");
		addText("In order to make a job advancement, you MUST head to Victoria Island. ");
		addText("Head to the " .. blue("Nautilus") .. ", a strange-looking submarine currently docked on the island, head inside, and find " .. blue("Kyrin") .. ". ");
		addText("She'll help you with the rest.");
		sendNext();

		addText("By the way! ");
		addText("Don't forget you have to be over level 10 and over 20 DEX to become a pirate!!");
		sendBackNext();
	end),
	makeChoiceHandler(" How do I raise the character stats? (S)", function()
		addText("You want to know how to raise your character's ability stats? ");
		addText("First press S to check out the ability window. ");
		addText("Every time you level up, you'll be awarded 5 ability points aka AP's. ");
		addText("Assign those AP's to the ability of your choice. ");
		addText("It's that simple.");
		sendNext();

		addText("Place your mouse cursor on top of all abilities for a brief explanation. ");
		addText("For example, STR for warriors, DEX for bowman, INT for magician, and LUK for thief. ");
		addText("That itself isn't everything you need to know, so you'll need to think long and hard on how to emphasize your character's strengths through assigning the points.");
		sendBackNext();
	end),
	makeChoiceHandler(" How do I check the items that I just picked up?", function()
		addText("You want to know how to check out the items you've picked up, huh? ");
		addText("When you defeat a monster, it'll drop an item on the ground, and you may press Z to pick up the item. ");
		addText("That item will then be stored in your item inventory, and you can take a look at it by simply press I.");
		sendNext();
	end),
	makeChoiceHandler(" How do I put on an item?", function()
		addText("You want to know how to wear the items, right? ");
		addText("Press I to check out your item inventory. ");
		addText("Place your mouse cursor on top of an item and double-click on it to put it on your character. ");
		addText("If you find yourself unable to wear the item, chances are your character does not meet the level & stat requirements. ");
		addText("You can also put on the item by opening the equipment inventory (E) and dragging the item into it. ");
		addText("To take off an item, double-click on the item at the equip. inventory.");
		sendNext();
	end),
	makeChoiceHandler(" How do I check out the items that I'm wearing?", function()
		addText("You want to check on the equipped items, right? ");
		addText("Press E to open the equipment inventory, where you'll see exactly what you are wearing right at that moment. ");
		addText("To take off an item, double-click on the item. ");
		addText("The item will then be sent to the item inventory.");
		sendNext();
	end),
	makeChoiceHandler(" What are skills?(K)", function()
		addText("The special 'abilities' you get after acquiring a job are called skills. ");
		addText("You'll acquire skills that are specifically for that job. ");
		addText("You're not at that stage yet, so you don't have any skills yet, but just remember that to check on your skills, press K to open the skill book. ");
		addText("It'll help you down the road.");
		sendNext();
	end),
	makeChoiceHandler(" How do I get to Victoria Island?", function()
		addText("You can head over to Victoria Island through a ship ride from Southperry that heads to Lith Harbor. ");
		addText("Press W to see the World Map, and you'll see where you are on the island. ");
		addText("Locate Southperry and that's where you'll need to go. ");
		addText("You'll also need some mesos for the ride, so you may need to hunt some monsters around here.");
		sendNext();
	end),
	makeChoiceHandler(" What are mesos?", function()
		addText("It's the currency used in MapleStory. ");
		addText("You may purchase items through mesos. ");
		addText("To earn them, you may either defeat the monsters, sell items at the store, or complete quests..");
		sendNext();
	end),
};

while true do
	addText("Now...ask me any questions you may have on traveling!!\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	if selectChoice(choices, choice) == iterationBreak then
		break;
	end
end
