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
-- Chief Tatamo

addText("...Can I help you?\r\n");
addText("#b#L0# Buy the Magic Seed#l\r\n");
addText("#L1# Do something for Leafre#l\r\n");
choice = askChoice();

affinity = getPlayerVariable("tatamo_affinity", true);
if not affinity then
	affinity = 0;
end

if choice == 0 then
	preAskText = nil;
	askText = nil;
	preAskNumberText = nil;
	askNumberText = nil;
	price = nil;
	partingText = "See you again~";

	if affinity < 5000 then
		askText = "You don't seem to be from our town. How can I help you?\r\n#b#L0# I would like some Magic Seed.#l";
		askNumberText = "#bMagic Seed#k is a precious item; I cannot give it to you just like that. How about doing me a little favor? Then I'll give it to you. I'll sell the #bMagic Seed#k to you for #b30,000 mesos#k each.\r\nAre you willing to make the purchase? How many would you like, then?";
		price = 30000;
	elseif affinity < 24000 then
		askText = "Haven't we met before? No wonder you looked familiar. Hahaha...\r\nHow can I help you this time?\r\n#b#L0# I would like some Magic Seed.#l";
		preAskNumberText = "Ahh~ now I remember. If I'm mistaken, I gave you some #bMagic Seed#k before. How was it? I'm guessing you are more than satisfied with your previous purchase based on the look on your face.";
		askNumberText = "#bMagic Seed#k is a precious item; I cannot give it to you just like that. How about doing me a little favor? Then I'll give it to you. I'll sell the #bMagic Seed#k to you for #b30,000 mesos#k each.\r\nAre you willing to make the purchase? How many would you like, then?";
		price = 27000;
	elseif affinity < 50000 then
		askText = "It's a beautiful day again today. Days like this should be spent out in the park on a picnic with your family. I have to admit, when I first met you, I had my reservations, what with you not being from this town and all ... but now, I feel more than comfortable doing business with you.\r\nHow can I help you this time?\r\n#b#L0# I would like some Magic Seed.#l";
		askNumberText = "#bMagic Seed#k is a rare, precious item indeed, but now that we have been acquainted for quite some time, I'll give you a special discount. How about #b24,000 mesos#k for a #bMagic Seed#k? It's cheaper than flying over here through the ship! How many would you like?";
		price = 24000;
	elseif affinity < 200000 then
		askText = "Hmmm ... It seems like Birk is crying out loud much louder than usual today. When Birk cries, it signals the fact that the egg of the baby dragon is ready to be hatched any minute now. Now that you have become part of the family here, I would like for you to personally witness the birth of the baby dragon when that time comes. \r\nDo you need something from me today?\r\n#b#L0# I would like some Magic Seed.#l";
		askNumberText = "You must have run out of the #bMagic Seed#k. We have grown very close to one another, and it doesn't sound too good for me to ask you for something in return, but please understand that the #bMagic Seed#k is very rare and hard to come by. How about #b18,000 mesos#k for #b1 Magic Seed#k? How many would you like to get?";
		price = 18000;
	elseif affinity < 800000 then
		preAskText = "Ohh hoh. I had a feeling that you'd be coming here right about now ...\r\nanyway, a while ago, a huge war erupted at the Dragon Shrine, where the dragons reside. Did you hear anything about it?";
		askText = "The sky shook, and the ground trembled as this incredibly loud thud covered every part of the forest. The baby dragons are now shivering in fear, wondering what may happen next. I wonder what actually happened... anyway, you're here for the seed, right?\r\n#b#L0# I would like some Magic Seed.#l";
		askNumberText = "I knew it. I can now tell just by looking at your eyes. I know that you will always be there for us here. We both understand that the #bMagic Seed#k is a precious item, but for you, I'll sell it to you for #b12,000 mesos#k. How many would you like?";
		price = 12000;
	else
		askText = "Aren't you here for the Magic Seed? A lot of time has passed since we first met, and now I feel a sense of calmness and relief whenever I talk to you. People in this town love you, and I think the same way about you. You're a true friend.\r\n#b#L0# Thank you so much for such kind words. I'd love to get some Magic Seeds right now.#l";
		askNumberText = "You know I always have them ready. Just give me #b8,000 mesos#k per seed. We've been friends for a while, anyway. How many would you like?";
		partingText = "By the way, have you heard of the news that the Head Dragon, the one that has been protecting the forest, has disappeared? Something's not right, if you ask me; I can tell by hearing the devilish cry of the Horntail all the way here. This greatly concerns me...";
		price = 8000;
	end

	if preAskText ~= nil then
		addText(preAskText);
		sendNext();
	end
	addText(askText);
	selection = askChoice();

	if selection == 0 then
		if preAskNumberText ~= nil then
			addText(preAskNumberText);
			sendNext();
		end

		addText(askNumberText);
		result = askNumber(0, 0, 100);

		if result == 0 then
			addText("I can't sell you 0.");
			sendNext();
		else
			totalPrice = price * result;
			addText("Buying #b" .. result .. " Magic Seed(s)#k will cost you #b" .. totalPrice .. " mesos#k.");
			addText("Are you sure you want to make the purchase?");
			verify = askYesNo();

			if verify == 0 then
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

					addText(partingText);
					sendNext();
				end
			end
		end
	end
elseif choice == 1 then
	text = nil;
	if affinity < 5000 then
		text = "It is the chief's duty to make the town more hospitable for people to live in, and carrying out the duty will require lots of items. If you have collected items around Leafre, are you interested in donating them?";
	elseif affinity < 24000 then
		text = "You're the person that graciously donated some great items to us before. I cannot tell you how helpful that really was. By the way, if you have collected items around Leafre, are you interested in donating them to us once more?";
	elseif affinity < 50000 then
		text = "You came to see me again today. Thanks to your immense help, the quality of life in this town has been significantly upgraded. People in this town are very thankful of your contributions. By the way, if you have collected items around Leafre, are you interested in donating them to us once more?";
	elseif affinity < 200000 then
		text = "Hey, there! Your tremendous contribution to this town has resulted in our town thriving like no other. The town is doing really well as it is, but I'd appreciate it if you can help us out again. If you have collected items around Leafre, are you interested in donating them to us once more?";
	elseif affinity < 800000 then
		text = "It's you, the number 1 supporter of Leafre! Good things always seem to happen when you're in town. By the way, if you have collected items around Leafre, are you interested in donating them to us once more?";
	else
		text = "Aren't you #b" .. getName() .. "#k? It's great to see you again! Thanks to your incredible work, our town is doing so well that I really don't have much to do these days. Everyone in this town seems to look up to you, and I mean that. I thoroughly appreciate your great help, but ... can you help us out once more? If you have collected items around Leafre, then would you be again interested in donating the items to us?";
	end

	addText(text);
	sendNext();

	itemsAndWorths = {
		[0] = {["item"] = 4000226, ["worth"] = 2},
		[1] = {["item"] = 4000229, ["worth"] = 4},
		[2] = {["item"] = 4000236, ["worth"] = 3},
		[3] = {["item"] = 4000237, ["worth"] = 6},
		[4] = {["item"] = 4000260, ["worth"] = 3},
		[5] = {["item"] = 4000261, ["worth"] = 6},
		[6] = {["item"] = 4000231, ["worth"] = 7},
		[7] = {["item"] = 4000238, ["worth"] = 9},
		[8] = {["item"] = 4000239, ["worth"] = 12},
		[9] = {["item"] = 4000241, ["worth"] = 15},
		[10] = {["item"] = 4000242, ["worth"] = 20},
		[11] = {["item"] = 4000234, ["worth"] = 20},
		[12] = {["item"] = 4000232, ["worth"] = 20},
		[13] = {["item"] = 4000233, ["worth"] = 20},
		[14] = {["item"] = 4000235, ["worth"] = 100},
		[15] = {["item"] = 4000243, ["worth"] = 100}
	};

	addText("Which item would you like to donate?\r\n#b");
	for i = 0, #itemsAndWorths - 1 do
		addText("#L" .. i .. "# #t" .. itemsAndWorths[i + 1]["item"] .. "##l\r\n");
	end

	itemChoice = askChoice();
	itemAndWorth = itemsAndWorths[itemChoice + 1];
	item = itemAndWorth["item"];
	worth = itemAndWorth["worth"];
	amount = getItemAmount(item);

	if amount == 0 then
		addText("I don't think you have the item.");
		sendNext();
	else
		addText("How many #b#t" .. item .. "##k's would you like to donate?\r\n");
		addText("#b< Owned : " .. amount .. " >#k");
		donation = askNumber(0, 0, amount);

		addText("Are you sure you want to donate #b" .. donation .. " #t" .. item .. "##k?");
		verify = askYesNo();

		if verify == 0 then
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
end