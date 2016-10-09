--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Chief Tatamo

dofile("scripts/utils/npcHelper.lua");

affinity = getPlayerVariable("tatamo_affinity", type_int);
if affinity == nil then
	affinity = 0;
end

choices = {
	makeChoiceHandler(" Buy the Magic Seed", function()
		choiceText = nil;
		if affinity < 5000 then
			addText("You don't seem to be from our town. ");
			addText("How can I help you?\r\n");
			choiceText = " I would like some Magic Seed.";
		elseif affinity < 24000 then
			addText("Haven't we met before? ");
			addText("No wonder you looked familiar. ");
			addText("Hahaha...\r\n");
			addText("How can I help you this time?\r\n");
			choiceText = " I would like some Magic Seed.";
		elseif affinity < 50000 then
			addText("It's a beautiful day again today. ");
			addText("Days like this should be spent out in the park on a picnic with your family. ");
			addText("I have to admit, when I first met you, I had my reservations, what with you not being from this town and all ... but now, I feel more than comfortable doing business with you.\r\n");
			addText("How can I help you this time?\r\n");
			choiceText = " I would like some Magic Seed.";
		elseif affinity < 200000 then
			addText("Hmmm ... ");
			addText("It seems like Birk is crying out loud much louder than usual today. ");
			addText("When Birk cries, it signals the fact that the egg of the baby dragon is ready to be hatched any minute now. ");
			addText("Now that you have become part of the family here, I would like for you to personally witness the birth of the baby dragon when that time comes. \r\n");
			addText("Do you need something from me today?\r\n");
			choiceText = " I would like some Magic Seed.";
		elseif affinity < 800000 then
			addText("Ohh hoh. ");
			addText("I had a feeling that you'd be coming here right about now ...\r\n");
			addText("anyway, a while ago, a huge war erupted at the Dragon Shrine, where the dragons reside. ");
			addText("Did you hear anything about it?");
			sendNext();

			addText("The sky shook, and the ground trembled as this incredibly loud thud covered every part of the forest. ");
			addText("The baby dragons are now shivering in fear, wondering what may happen next. ");
			addText("I wonder what actually happened... anyway, you're here for the seed, right?\r\n");
			choiceText = " I would like some Magic Seed.";
		else
			addText("Aren't you here for the Magic Seed? ");
			addText("A lot of time has passed since we first met, and now I feel a sense of calmness and relief whenever I talk to you. ");
			addText("People in this town love you, and I think the same way about you. ");
			addText("You're a true friend.\r\n");
			choiceText = " Thank you so much for such kind words. I'd love to get some Magic Seeds right now.";
		end

		addText(blue(choiceRef(choiceText)));
		choice = askChoice();

		price = nil;
		if affinity < 5000 then
			price = 30000;

			addText(blue("Magic Seed") .. " is a precious item; I cannot give it to you just like that. ");
			addText("How about doing me a little favor? ");
			addText("Then I'll give it to you. ");
			addText("I'll sell the " .. blue("Magic Seed") .. " to you for " .. blue("30,000 mesos") .. " each.\r\n");
			addText("Are you willing to make the purchase? ");
			addText("How many would you like, then?");
		elseif affinity < 24000 then
			price = 27000;

			addText("Ahh~ now I remember. ");
			addText("If I'm mistaken, I gave you some " .. blue("Magic Seed") .. " before. ");
			addText("How was it? ");
			addText("I'm guessing you are more than satisfied with your previous purchase based on the look on your face.");
			sendNext();

			addText(blue("Magic Seed") .. " is a precious item; I cannot give it to you just like that. ");
			addText("How about doing me a little favor? ");
			addText("Then I'll give it to you. ");
			addText("I'll sell the " .. blue("Magic Seed") .. " to you for " .. blue("30,000 mesos") .. " each.\r\n");
			addText("Are you willing to make the purchase? ");
			addText("How many would you like, then?");
		elseif affinity < 50000 then
			price = 24000;

			addText(blue("Magic Seed") .. " is a rare, precious item indeed, but now that we have been acquainted for quite some time, I'll give you a special discount. ");
			addText("How about " .. blue("24,000 mesos") .. " for a " .. blue("Magic Seed") .. "? ");
			addText("It's cheaper than flying over here through the ship! ");
			addText("How many would you like?");
		elseif affinity < 200000 then
			price = 18000;

			addText("You must have run out of the " .. blue("Magic Seed") .. ". ");
			addText("We have grown very close to one another, and it doesn't sound too good for me to ask you for something in return, but please understand that the " .. blue("Magic Seed") .. " is very rare and hard to come by. ");
			addText("How about " .. blue("18,000 mesos") .. " for " .. blue("1 Magic Seed") .. "? ");
			addText("How many would you like to get?");
		elseif affinity < 800000 then
			price = 12000;

			addText("I knew it. ");
			addText("I can now tell just by looking at your eyes. ");
			addText("I know that you will always be there for us here. ");
			addText("We both understand that the " .. blue("Magic Seed") .. " is a precious item, but for you, I'll sell it to you for " .. blue("12,000 mesos") .. ". ");
			addText("How many would you like?");
		else
			price = 8000;

			addText("You know I always have them ready. ");
			addText("Just give me " .. blue("8,000 mesos") .. " per seed. ");
			addText("We've been friends for a while, anyway. ");
			addText("How many would you like?");
		end

		result = askNumber(0, 0, 100);

		if result == 0 then
			addText("I can't sell you 0.");
			sendNext();
		else
			totalPrice = price * result;
			addText("Buying " .. blue(result .. " Magic Seed(s)") .. " will cost you " .. blue(totalPrice .. " mesos") .. ". ");
			addText("Are you sure you want to make the purchase?");
			answer = askYesNo();

			if answer == answer_no then
				addText("Please think carefully. ");
				addText("Once you have made your decision, let me know.");
				sendNext();
			else
				item = 4031346;
				if getMesos() < totalPrice or not hasOpenSlotsFor(item, result) then
					addText("Please check and see if you have enough mesos to make the purchase. ");
					addText("Also, I suggest you check the etc. inventory and see if you have enough space available to make the purchase.");
					sendNext();
				else
					giveMesos(-totalPrice);
					giveItem(item, result);

					if affinity >= 800000 then
						addText("By the way, have you heard of the news that the Head Dragon, the one that has been protecting the forest, has disappeared? ");
						addText("Something's not right, if you ask me; I can tell by hearing the devilish cry of the Horntail all the way here. ");
						addText("This greatly concerns me...");
					else
						addText("See you again~");
					end
					sendNext();
				end
			end
		end
	end),
	makeChoiceHandler(" Do something for Leafre", function()
		if affinity < 5000 then
			addText("It is the chief's duty to make the town more hospitable for people to live in, and carrying out the duty will require lots of items. ");
			addText("If you have collected items around Leafre, are you interested in donating them?");
		elseif affinity < 24000 then
			addText("You're the person that graciously donated some great items to us before. ");
			addText("I cannot tell you how helpful that really was. ");
			addText("By the way, if you have collected items around Leafre, are you interested in donating them to us once more?");
		elseif affinity < 50000 then
			addText("You came to see me again today. ");
			addText("Thanks to your immense help, the quality of life in this town has been significantly upgraded. ");
			addText("People in this town are very thankful of your contributions. ");
			addText("By the way, if you have collected items around Leafre, are you interested in donating them to us once more?");
		elseif affinity < 200000 then
			addText("Hey, there! ");
			addText("Your tremendous contribution to this town has resulted in our town thriving like no other. ");
			addText("The town is doing really well as it is, but I'd appreciate it if you can help us out again. ");
			addText("If you have collected items around Leafre, are you interested in donating them to us once more?");
		elseif affinity < 800000 then
			addText("It's you, the number 1 supporter of Leafre! ");
			addText("Good things always seem to happen when you're in town. ");
			addText("By the way, if you have collected items around Leafre, are you interested in donating them to us once more?");
		else
			addText("Aren't you " .. blue(getName()) .. "? ");
			addText("It's great to see you again! ");
			addText("Thanks to your incredible work, our town is doing so well that I really don't have much to do these days. ");
			addText("Everyone in this town seems to look up to you, and I mean that. ");
			addText("I thoroughly appreciate your great help, but ... can you help us out once more? ");
			addText("If you have collected items around Leafre, then would you be again interested in donating the items to us?");
		end

		sendNext();

		function generateChoice(itemId, worth)
			return makeChoiceData(" " .. itemRef(itemId), {itemId, worth});
		end

		choices = {
			generateChoice(4000226, 2),
			generateChoice(4000229, 4),
			generateChoice(4000236, 3),
			generateChoice(4000237, 6),
			generateChoice(4000260, 3),
			generateChoice(4000261, 6),
			generateChoice(4000231, 7),
			generateChoice(4000238, 9),
			generateChoice(4000239, 12),
			generateChoice(4000241, 15),
			generateChoice(4000242, 20),
			generateChoice(4000234, 20),
			generateChoice(4000232, 20),
			generateChoice(4000233, 20),
			generateChoice(4000235, 100),
			generateChoice(4000243, 100),
		};

		addText("Which item would you like to donate?\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		data = selectChoice(choices, choice);
		item, worth = data[1], data[2];
		amount = getItemAmount(item);

		if amount == 0 then
			addText("I don't think you have the item.");
			sendNext();
		else
			addText("How many " .. blue(itemRef(item)) .. "'s would you like to donate?\r\n");
			addText(blue("< Owned : " .. amount .. " >"));
			donation = askNumber(0, 0, amount);

			addText("Are you sure you want to donate " .. blue(donation .. " " .. itemRef(item)) .. "?");
			answer = askYesNo();

			if answer == answer_no then
				addText("Think about it, and then let me know of your decision.");
				sendNext();
			else
				if getItemAmount(item) < amount then
					addText("Please check and see if you have enough of the item.");
					sendNext();
				else
					affinity = affinity + worth * donation;
					if affinity > 800000 then
						affinity = 800000;
					end
					setPlayerVariable("tatamo_affinity", affinity);

					addText("Thank you very much.");
					sendNext();
				end
			end
		end
	end),
};

addText("...Can I help you?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);