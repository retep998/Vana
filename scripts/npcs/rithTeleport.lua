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
-- Phil (Lith Harbor cab)

dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceHandler("What kind of towns are here in Victoria Island?", function()
		choices = {
			makeChoiceHandler(mapRef(104000000), function()
				addText("The town you are at is Lith Harbor! ");
				addText("Alright I'll explain to you more about " .. blue(mapRef(104000000)) .. ". ");
				addText("It's the place you landed on Victoria Island by riding The Victoria. ");
				addText("That's " .. mapRef(104000000) .. ". ");
				addText("A lot of beginners who just got here from Maple Island start their journey here.");
				sendNext();

				addText("It's a quiet town with the wide body of water on the back of it, thanks to the fact that the harbor is located at the west end of the island. ");
				addText("Most of the people here are, or used to be fishermen, so they may look intimidating, but if you strike up a conversation with them, they'll be friendly to you.");
				sendBackNext();

				addText("Around town lies a beautiful prairie. ");
				addText("Most of the monsters there are small and gentle, perfect for beginners. ");
				addText("If you haven't chosen your job yet, this is a good place to boost up your level.");
				sendBackNext();
			end),
			makeChoiceHandler(mapRef(102000000), function()
				addText("Alright I'll explain to you more about " .. blue(mapRef(102000000)) .. ". ");
				addText("It's a warrior-town located at the northern-most part of Victoria Island, surrounded by rocky mountains. ");
				addText("With an unfriendly atmosphere, only the strong survives there.");
				sendNext();

				addText("Around the highland you'll find a really skinny tree, a wild hog running around the place, and monkeys that live all over the island. ");
				addText("There's also a deep valley, and when you go deep into it, you'll find a humongous dragon with the power to match his size. ");
				addText("Better go in there very carefully, or don't go at all.");
				sendBackNext();

				addText("If you want to be a the " .. blue("Warrior") .. " then find " .. red(npcRef(1022000)) .. ", the chief of " .. mapRef(102000000) .. ". ");
				addText("If you're level 10 or higher, along with a good STR level, he may make you a warrior afterall. ");
				addText("If not, better keep training yourself until you reach that level.");
				sendBackNext();
			end),
			makeChoiceHandler(mapRef(101000000), function()
				addText("Alright I'll explain to you more about " .. blue(mapRef(101000000)) .. ". ");
				addText("It's a magician-town located at the fart east of Victoria Island, and covered in tall, mystic trees. ");
				addText("You'll find some fairies there, too; They don't like humans in general so it'll be best for you to be on their good side and stay quiet.");
				sendNext();

				addText("Near the forest you'll find green slimes, walking mushrooms, monkeys and zombie monkeys all residing there. ");
				addText("Walk deeper into the forest and you'll find witches with the flying broomstick navigating the skies. ");
				addText("A word of warning: unless you are really strong, I recommend you don't go near them.");
				sendBackNext();

				addText("If you want to be the " .. blue("Magician") .. ", search for " .. red(npcRef(1032001)) .. ", the head wizard of " .. mapRef(101000000) .. ". ");
				addText("He may make you a wizard if you're at or above level 8 with a decent amount of INT. ");
				addText("If that's not the case, you may have to hunt more and train yourself to get there.");
				sendBackNext();
			end),
			makeChoiceHandler(mapRef(100000000), function()
				addText("Alright I'll explain to you more about " .. blue(mapRef(100000000)) .. ". ");
				addText("It's a bowman-town located at the southernmost part of the island, made on a flatland in the mIdst of a deep forest and prairies. ");
				addText("The weather's just right, and everything is plentiful around that town, perfect for living. ");
				addText("Go check it out.");
				sendNext();

				addText("Around the prairie you'll find weak monsters such as snails, mushrooms, and pigs. ");
				addText("According to what I hear, though, in the deepest part of the Pig Park, which is connected to the town somewhere, you'll find a humongous, powerful mushroom called Mushmom every now and then.");
				sendBackNext();

				addText("If you want to be the " .. blue("Bowman") .. ", you need to go see " .. red(npcRef(1012100)) .. " at " .. mapRef(100000000) .. ". ");
				addText("With a level at or above 10 and a decent amount of DEX, she may make you be one afterall. ");
				addText("If not, go train yourself, make yourself stronger, then try again.");
				sendBackNext();
			end),
			makeChoiceHandler(mapRef(103000000), function()
				addText("Alright I'll explain to you more about " .. blue(mapRef(103000000)) .. ". ");
				addText("It's a thief-town located at the northwest part of Victoria Island, and there are buildings up there that have just this strange feeling around them. ");
				addText("It's mostly covered in black clouds, but if you can go up to a really high place, you'll be able to see a very beautiful sunset there.");
				sendNext();

				addText("From " .. mapRef(103000000) .. ", you can go into several dungeons. ");
				addText("You can go to a swamp where alligators and snakes are abound, or hit the subway full of ghosts and bats. ");
				addText("At the deepest part of the underground, you'll find Lace, who is just as big and dangerous as a dragon.");
				sendBackNext();

				addText("If you want to be the " .. blue("Thief") .. ", seek " .. red(npcRef(1052001)) .. ", the heart of darkness of " .. mapRef(103000000) .. ". ");
				addText("He may well make you the thief if you're at or above level 10 with a good amount of DEX. ");
				addText("If not, go hunt and train yourself to reach there.");
				sendBackNext();
			end),
			makeChoiceHandler(mapRef(105040300), function()
				addText("Alright I'll explain to you more about " .. blue(mapRef(105040300)) .. ". ");
				addText("It's a forest town located at the southeast side of Victoria Island. ");
				addText("It's pretty much in between " .. mapRef(100000000) .. " and the ant-tunnel dungeon. ");
				addText("There's a hotel there, so you can rest up after a long day at the dungeon ... it's a quiet town in general.");
				sendNext();

				addText("In front of the hotel there's an old buddhist monk by the name of " .. red(npcRef(1061000)) .. ". ");
				addText("Nobody knows a thing about that monk. ");
				addText("Apparently he collects materials from the travellers and create something, but I am not too sure about the details. ");
				addText("If you have any business going around that area, please check that out for me.");
				sendBackNext();

				addText("From " .. mapRef(105040300) .. ", head east and you'll find the ant tunnel connected to the deepest part of the Victoria Island. ");
				addText("Lots of nasty, powerful monsters abound so if you walk in thinking it's a walk in the park, you'll be coming out as a corpse. ");
				addText("You need to fully prepare yourself for a rough ride before going in.");
				sendBackNext();

				addText("And this is what I hear ... apparently, at " .. mapRef(105040300) .. " there's a secret entrance leading you to an unknown place. ");
				addText("Apparently, once you move in deep, you'll find a stack of black rocks that actually move around. ");
				addText("I want to see that for myself in the near future ...");
				sendBackNext();
			end),
		};

		addText("There are 6 big towns here in Victoria Island. ");
		addText("Which of those do you want to know more of?\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end),
	makeChoiceHandler("Please take me somewhere else.", function()
		function generateChoice(mapId, price)
			return makeChoiceData(mapRef(mapId) .. "(" .. price .. " mesos) ", {mapId, price});
		end

		function getPrice(basePrice)
			if getJob() == 0 then
				return basePrice / 10;
			end
			return basePrice;
		end

		choices = {
			generateChoice(102000000, getPrice(1200)),
			generateChoice(101000000, getPrice(1200)),
			generateChoice(100000000, getPrice(800)),
			generateChoice(103000000, getPrice(1000)),
		};

		if getJob() == 0 then
			addText("There's a special 90% discount for all beginners. ");
			addText("Alright, where would you want to go? \r\n");
		else
			addText("Oh you aren't a beginner, huh? ");
			addText("Then I'm afraid I may have to charge you full price. ");
			addText("Where would you like to go?\r\n");
		end
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		data = selectChoice(choices, choice);
		mapId, price = data[1], data[2];

		addText("I guess you don't need to be here. ");
		addText("Do you really want to move to " .. blue(mapRef(mapId)) .."? ");
		addText("Well it'll cost you " .. blue(price .. " meso") .. ". ");
		addText("What do you think?");
		answer = askYesNo();

		if answer == answer_yes then
			if getMesos() >= price then
				giveMesos(-price);
				setMap(mapId);
			else
				addText("You don't have enough mesos. ");
				addText("With your abilities, you should have more than that!");
				sendOk();
			end
		else
			addText("There's alot to see in this town too. ");
			addText("Let me know if you want to go somewhere else.");
			sendOk();
		end
	end),
};

addText("Do you wanna head over to some other town? ");
addText("With a little money involved, I can make it happen. ");
addText("It's a tad expensive, but I run a special 90% discount for beginners.");
sendNext();

addText("It's understable that you may be confused about this place if this is your first time around. ");
addText("If you got any questions about this place, fire away. \r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);