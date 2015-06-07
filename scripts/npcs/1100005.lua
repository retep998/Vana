--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Kiruru (on the boat between Ereve and Ellinia)

dofile("scripts/utils/npcHelper.lua");

isToEreve = (getMap() % 2) == 0;
if isToEreve then
	choices = {
		makeChoiceHandler("Yes, please tell me.", function()
			addText("Ereve is the stronghold of the Cygnus Knights. ");
			addText("It is where the fair and beautiful Empress Cygnus trains the Cygnus Knights. ");
			addText("They say that the Empress is physically very weak and must be protected by Shinsoo.");
			sendNext();

			addText("Aside from Shinsoo, the tactician Neinheart, who assists the Empress, is also there. ");
			addText("He is famous for being able to spit out malicious criticism all while maintaining a smile. ");
			addText("They say that even the Chief Knights are careful around him, fearing cuts in their budget.");
			sendBackNext();

			addText("The Chief Knights also protect the Empress. ");
			addText("While the curt Mihile and the cold Eckhart don't really get along, the dense Oz and the righteous Irena are closer than you'd think. ");
			addText("Ah, and Hawkeye, well he would get along even with a Horntail.");
			sendBackNext();

			addText("Those of us who have blue feathers are of the Piyo race, a race that likes humans. ");
			addText("We can only live in high altitudes so we don't usually get the chance to meet humans. ");
			addText("But in Ereve, we enjoy befriending humans.");
			sendBackNext();

			addText(blue("(Kiriru continued to talk for a long time...)"));
			sendBackOk();
		end),
		makeChoiceHandler("No, it's okay.", function()
			-- Intentionally left blank
		end),
	};

	addText("The weather is so nice. ");
	addText("At this rate, we should arrive at Ereve in no time...");
	addText("Do you know much about Ereve? ");
	addText("It's a floating island protected by Shinsoo. ");
	addText("Would you like to know more about Ereve?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	choices = {
		makeChoiceHandler("Yes, please tell me.", function()
			choices = {
				makeChoiceHandler("Ellinia", function()
					addText("Ellinia is the town of Magicians located in the eastern region of Victoria Island. ");
					addText("They say it was the center of magic development in Maple World. ");
					addText("If you want to purchase equipment for magicians, visit the Ellinia Armor Shop.");
					sendNext();

					addText("Ellinia is also a town of Fairies. ");
					addText("You can meet the coy Arwen, the dignified but pleasant Rowen, or Wing, who always has an attitude, plus many others. ");
					addText("Come to think of it, I heard that Grendel the Really Old, the head magician in Ellinia, was raised by fairies...");
					sendBackNext();

					addText("(Kiriru continued to talk for a long time...)");
					sendBackNext();
				end),
				makeChoiceHandler("Henesys", function()
					addText("Henesys is a town favored by Bowmen, and it's located in the southern region of Victoria Island. ");
					addText("Henesys is protected by Athena Pierce. ");
					addText("If you want to purchase equipment for Bowmen, take a look at the Henesys Market.");
					sendNext();

					addText("Henesys is also the central hub of Victoria Island. ");
					addText("It's busy, loud, and full of people. ");
					addText("There's a good hunting ground for beginners nearby so it draws a lot of low level people.");
					sendBackNext();

					addText("The chief of Henesys is Stan. ");
					addText("He always has a huge headache caused by his runaway son and his wife, who's busy preparing for a party. ");
					addText("Though he may frown a lot, Roca says that he's actually a really nice guy.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
				makeChoiceHandler("Perion", function()
					addText("Perion is a town located in the northern region of Victoria Island. ");
					addText("It's a town of Warriors, so if you need equipment for Warriors, you should visit the armor shop there.");
					sendNext();

					addText("Perion is very dry, which causes monsters such as Stumps to appear in great numbers. ");
					addText("Have you ever seen a Stump? ");
					addText("It's a one-eyed monster shaped like a tree trunk. ");
					addText("It's fairly strong and annoying to fight.");
					sendBackNext();

					addText("The Warriors of Perion often name themselves in order to show their courage. ");
					addText("That's why some people have strange names, like 'Blackbull' or even 'Dances wth Balrog.' ");
					addText("They may sound silly but they are the mark of a strong Warrior.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
				makeChoiceHandler("Kerning City", function()
					addText("...since Kerning City is a town of Thieves, they are a bit secretive. ");
					addText("It's full of buildings that have unmarked entrance portals. ");
					addText("That doesn't mean you can't enter, though, so keep an eye out for secret entrances.");
					sendNext();

					addText("Kerning City has a great transportation system called the Subway. ");
					addText("Unfortunately, it's not in working order because of monsters. ");
					addText("But you can always have a look around... ");
					addText("Come to think of it, I've heard that the Kerning City sewers are connected to the swamps...but that's probably just a rumor.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
				makeChoiceHandler("Lith Harbor", function()
					addText("Lith Harbor is located in the southwest area of Victoria Island. ");
					addText("It's a harbor that ships can use, but since the ships only travel by sea and not by air, the actual places you can go there are very limited.");
					sendNext();

					addText("Still, from Lith Harbor you can visit Florina Beach, the only vacation spot in Maple World...but though they call it a vacation spot, it's difficult to get any rest there. ");
					addText("The beaches are full of the incredibly dangerous Lorangs and Clangs.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
				makeChoiceHandler("Nautilus", function()
					addText("You want to know about Nautilus? ");
					addText("The Nautilus is a submarine that is currently anchored in the southeastern region of Victoria Island. ");
					addText("It is where Captain Kyrin commands the Pirates. ");
					addText("If you need equipment for Pirates, you should find it in the armor shop or the weapons shop there.");
					sendNext();

					addText("The Nautilus looks like a huge whale and they say that it is often mistaken for a whale when it is moving. ");
					addText("It's supposed to be evidence of Omega Sector's great technological breakthroughs, but does it really need to look so similar to a whale...?");
					sendBackNext();

					addText("I've heard that Kyrin, the captain of the Nautilus, is a beautiful girl with orange hair. ");
					addText("I wonder if it's true. ");
					addText("I imagine she is pretty tough, dealing with Pirates and all... ");
					addText("Though they say that she is a beauty, she probably doesn't have a beautiful beak and feathers like Shinsoo.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
				makeChoiceHandler("Sleepywood", function()
					addText("There is a huge forest called Sleepywood in the center of Victoria Island and at its center is an entrance that leads to the Sleepy Dungeon. ");
					addText("They say that you can find those seeking enlightenment practicing asceticism at its entrance.");
					sendNext();

					addText("I hear there are all sorts of scary monsters in the underground dungeon. ");
					addText("And, in the center of it all is Balrog! ");
					addText("Eeek! ");
					addText("Aren't you frightened?");
					sendBackNext();

					addText("But I wonder if Balrog is the end of it. ");
					addText("The name Sleepywood would suggest that something is asleep there, don't you think? ");
					addText("Maybe there is still something there that hasn't awakened yet...");
					addText("Well, my fears are probably unfounded.");
					sendBackNext();

					addText(blue("(Kiriru continued to talk for a long time...)"));
					sendBackNext();
				end),
			};

			addText("Victoria Island is the biggest island in Maple World. ");
			addText("It's so big that you could actually call it a continent. ");
			addText("Despite its size, transportation is excellent since there's a " .. blue("Taxi") .. " in each town. ");
			addText("Traveling through Maple World can be fascinating because each town is so different. ");
			addText("Would you like me to tell you about each of Victoria Island's towns?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			selectChoice(choices, choice);
		end),
		makeChoiceHandler("No, it's okay.", function()
			addText("I guess you already know a lot about Victoria Island. ");
			addText("Have a good trip...");
			sendNext();
		end),
	};

	addText("The weather is so nice. ");
	addText("At this rate, we'll get to Victoria Island in no time...");
	addText("Is this your first time going to Victoria Island? ");
	addText("If so, would you like me to tell you a little but about the place?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
end