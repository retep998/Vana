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
-- Cloy, explains all about pets

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

enableApTransfer = false;

addText("Hmm... are you raising one of my kids by any chance? ");
addText("I perfected a spell that uses Water of Life to blow life into a doll. ");
addText("People call it the " .. blue("Pet") .. ". ");
addText("If you have one with you, feel free to ask me questions.");
sendNext();

function petCommand(commandList, minLevel, maxLevel)
	if minLevel == nil then
		minLevel = 1;
	end
	if maxLevel == nil then
		maxLevel = 30;
	end
	if minLevel == maxLevel then
		return blue(commandList) .. "(level " .. maxLevel .. ")";
	end
	return blue(commandList) .. "(level " .. minLevel .. " ~ " .. maxLevel .. ")";
end

function commandList(commands)
	text = "";
	for i = 1, #commands do
		if i > 1 then
			text = text .. "\r\n";
		end
		text = text .. commands[i];
	end
	return text;
end

choices = {
	makeChoiceHandler("Tell me more about Pets.", function()
		addText("So you want to know more about Pets. ");
		addText("Long ago I made a doll, sprayed Water of Life on it, and cast a spell on it to create a magical animal. ");
		addText("I know it sounds unbelievable, but it's a doll that became an actual living thing. ");
		addText("They understand and follow people very well.");
		sendNext();

		addText("But Water of Life only comes out a little at the very bottom of the World Tree, so I can't give him too much time in life ... ");
		addText("I know, it's very unfortunate ... but even if it becomes a doll again I can always bring life back into it so be good to it while you're with it.");
		sendBackNext();

		addText("Oh yeah, they'll react when you give them special commands. ");
		addText("You can scold them, love them ... it all depends on how you take care of them. ");
		addText("They are afraid to leave their masters so be nice to them, show them love. ");
		addText("They can get sad and lonely fast...");
		sendOk();
	end),
	makeChoiceHandler("How Do I raise Pets?", function()
		addText("Depending on the command you give, pets can love it, hate, and display other kinds of reactions to it. ");
		addText("If you give the pet a command and it follows you well, your intimacy goes up. ");
		addText("Double click on the pet and you can check the intimacy, level, fullness and etc..");
		sendNext();

		addText("Talk to the pet, pay attention to it and its intimacy level will go up and eventually his overall level will go up too. ");
		addText("As the intimacy level rises, the pet's overall level rises, one day the pet may even talk like a person a little bit, so try hard raising it. ");
		addText("Of course it won't be easy doing so...");
		sendBackNext();

		addText("It may be a live doll but they also have life so they can feel the hunger too. ");
		addText(blue("Fullness") .. " shows the level of hunger the pet's in. ");
		addText("100 is the max, and the lower it gets, it means that the pet is getting hungrier. ");
		addText("After a while, it won't even follow your command and be on the offensive, so watch out over that.");
		sendBackNext();

		addText("Oh, and if you don't feed the pet for a long period of time, it goes back home by itself. ");
		addText("You can take it out of its home and feed it but it's not really good for the pet's health, so try feeding him on a regular basis so it doesn't go down to that level, alright? ");
		addText("I think this will do.");
		sendOk();
	end),
	makeChoiceHandler("Do Pets die too?", function()
		addText("Dying ... well, they aren't technically ALIVE per se, so I don't know if dying is the right term to use. ");
		addText("They are dolls with my magical power and the power of Water of Life to become a live object. ");
		addText("Of course while it's alive, it's just like a live animal...");
		sendNext();

		addText("After some time ... that's correct, they stop moving. ");
		addText("They just turn back to being a doll, after the effect of magic dies down and the Water of Life dries out. ");
		addText("But that doesn't mean it's stopped forever, because once you pour Water of Life over it again, it's going to be back alive.");
		sendBackNext();

		addText("Even if it someday moves again, it's sad to see them stop altogether. ");
		addText("Please be nice to them while they are alive and moving. ");
		addText("Feed them well, too. ");
		addText("Isn't it nice to know that there's something alive that follows and listens to only you?");
		sendOk();
	end),
};

if isValidItem(5000000) then
	append(choices, makeChoiceHandler("What are the commands for Brown and Black Kitty?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("iloveyou"),
			petCommand("poop"),
			petCommand("talk, say, chat", 10),
			petCommand("cutie", 10),
			petCommand("up, stand, rise", 10),
		};

		addText("These are the commands for " .. red("Brown Kitty and Black Kitty") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000001) then
	append(choices, makeChoiceHandler("What are the commands for Brown Puppy?", function()
		commands = {
			petCommand("sit"),
			petCommand("stupid, ihateyou, baddog, dummy"),
			petCommand("iloveyou"),
			petCommand("pee"),
			petCommand("talk, say, chat", 10),
			petCommand("down", 10),
			petCommand("up, stand, rise", 20),
		};

		addText("These are the commands for " .. red("Brown Puppy") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000005) then
	append(choices, makeChoiceHandler("What are the commands for Pink and White Bunny?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("up, stand, rise"),
			petCommand("iloveyou"),
			petCommand("poop"),
			petCommand("talk, say, chat", 10),
			petCommand("hug", 10),
			petCommand("sleep, sleepy, gotobed", 20),
		};

		addText("These are the commands for " .. red("Pink Bunny and White Bunny") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000003) then
	append(choices, makeChoiceHandler("What are the commands for Mini Kargo?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("up, stand, rise"),
			petCommand("iloveyou"),
			petCommand("pee"),
			petCommand("talk, say, chat", 10),
			petCommand("thelook, charisma", 10),
			petCommand("goodboy, goodgirl", 20),
		};

		addText("These are the commands for " .. red("Mini Kargo") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000014) then
	append(choices, makeChoiceHandler("What are the commands for Rudolph, Dasher?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("up, stand"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("merryxmas, merrychristmas"),
			petCommand("iloveyou"),
			petCommand("poop"),
			petCommand("talk, say, chat", 11),
			petCommand("lonely, alone", 11),
			petCommand("cutie", 11),
			petCommand("mush, go", 21),
		};

		addText("These are the commands for " .. red("Rudolph, Dasher") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000007) then
	append(choices, makeChoiceHandler("What are the commands for Black Pig?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("poop"),
			petCommand("iloveyou"),
			petCommand("hand"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("talk, say, chat", 10),
			petCommand("smile", 10),
			petCommand("thelook, charisma", 20),
		};

		addText("These are the commands for " .. red("Black Pig") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000008) then
	append(choices, makeChoiceHandler("What are the commands for Panda?", function()
		commands = {
			petCommand("sit"),
			petCommand("chill, relax"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("poop"),
			petCommand("iloveyou"),
			petCommand("up, stand, rise"),
			petCommand("talk, chat, say", 10),
			petCommand("letsplay", 10),
			petCommand("meh, bleh", 10),
			petCommand("sleep", 20),
		};

		addText("These are the commands for " .. red("Panda") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000006) then
	append(choices, makeChoiceHandler("What are the commands for Husky?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("stupid, ihateyou, baddog, dummy"),
			petCommand("hand"),
			petCommand("poop"),
			petCommand("iloveyou"),
			petCommand("down", 10),
			petCommand("talk, chat, say", 10),
			petCommand("up, rise, stand", 20),
		};

		addText("These are the commands for " .. red("Husky") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000009) then
	append(choices, makeChoiceHandler("What are the commands for Dino boy, Dino girl?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("iloveyou"),
			petCommand("poop"),
			petCommand("smile, laugh"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("talk, chat, say", 10),
			petCommand("cutie", 10),
			petCommand("sleep, nap, sleepy", 20),
		};

		addText("These are the commands for " .. red("Dino boy, Dino girl") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000011) then
	append(choices, makeChoiceHandler("What are the commands for Monkey?", function()
		commands = {
			petCommand("sit"),
			petCommand("rest"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("pee"),
			petCommand("iloveyou"),
			petCommand("up, stand"),
			petCommand("talk, say, chat", 10),
			petCommand("play", 10),
			petCommand("melong", 10),
			petCommand("sleep, gotobed, sleepy", 20),
		};

		addText("These are the commands for " .. red("Monkey") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000022) then
	append(choices, makeChoiceHandler("What are the commands for Turkey?", function()
		commands = {
			petCommand("sit"),
			petCommand("no, rudeboy, mischief"),
			petCommand("stupid"),
			petCommand("iloveyou"),
			petCommand("up, stand"),
			petCommand("talk, chat, gobble", 10),
			petCommand("yes, goodboy", 10),
			petCommand("sleepy, birdnap, doze", 20),
			petCommand("birdeye, thanksgiving, fly, friedbird, imhungry", 30),
		};

		addText("These are the commands for " .. red("Turkey") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000012) then
	append(choices, makeChoiceHandler("What are the commands for White tiger?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("iloveyou"),
			petCommand("poop"),
			petCommand("rest, chill"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("talk, chat, say", 10),
			petCommand("actsad, sadlook", 10),
			petCommand("wait", 20),
		};

		addText("These are the commands for " .. red("White Tiger") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000023) then
	append(choices, makeChoiceHandler("What are the commands for Penguin?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("poop"),
			petCommand("up, stand, rise"),
			petCommand("iloveyou"),
			petCommand("talk, say, chat"),
			petCommand("hug, hugme", 10),
			petCommand("wing, hand", 10),
			petCommand("sleep", 20),
			petCommand("kiss, smooch, muah", 20),
			petCommand("fly", 20),
			petCommand("cute, adorable", 20),
		};

		addText("These are the commands for " .. red("Penguin") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000025) then
	append(choices, makeChoiceHandler("What are the commands for Golden Pig?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("poop"),
			petCommand("iloveyou"),
			petCommand("talk, chat say", 11),
			petCommand("loveme, hugme", 11),
			petCommand("sleep, sleepy, gotobed", 21),
			petCommand("ignore / impressed / outofhere", 21),
			petCommand("roll, showmethemoney", 21),
		};

		addText("These are the commands for " .. red("Golden Pig") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000017) then
	append(choices, makeChoiceHandler("What are the commands for Robot?", function()
		commands = {
			petCommand("sit"),
			petCommand("up, stand, rise"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("attack, charge"),
			petCommand("iloveyou"),
			petCommand("good, thelook, charisma", 11),
			petCommand("speak, talk, chat, say", 11),
			petCommand("disguise, change, transform", 11),
		};

		addText("These are the commands for " .. red("Robot") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000020) then
	append(choices, makeChoiceHandler("What are the commands for Mini Yeti?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badboy, badgirl"),
			petCommand("poop"),
			petCommand("dance, boogie, shakeit"),
			petCommand("cute, cutie, pretty, adorable"),
			petCommand("iloveyou, likeyou, say"),
			petCommand("talk, say, chat", 11),
			petCommand("sleep, nap, sleepy, gotobed", 11),
		};

		addText("These are the commands for " .. red("Mini Yeti") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000024) then
	append(choices, makeChoiceHandler("What are the commands for Jr. Balrog?", function()
		commands = {
			petCommand("liedown"),
			petCommand("no, bad, badgirl, badboy"),
			petCommand("iloveyou, mylove, likeyou"),
			petCommand("cute, cutie, pretty, adorable"),
			petCommand("poop"),
			petCommand("smirk, crooked, laugh"),
			petCommand("melong", 11),
			petCommand("good, thelook, charisma", 11),
			petCommand("speak, talk, chat, say", 11),
			petCommand("sleep, nap, sleepy", 11),
			petCommand("gas", 21),
		};

		addText("These are the commands for " .. red("Jr. Balrog") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000029) then
	append(choices, makeChoiceHandler("What are the commands for Baby Dragon?", function()
		commands = {
			petCommand("sit"),
			petCommand("no, bad, badgirl, badboy"),
			petCommand("iloveyou, loveyou"),
			petCommand("poop"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("cutie", 11),
			petCommand("talk, say, chat", 11),
			petCommand("sleep, sleepy, gotobed", 11),
		};

		addText("These are the commands for " .. red("Baby Dragon") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000030) then
	append(choices, makeChoiceHandler("What are the commands for Green/Blue/Red Dragon?", function()
		commands = {
			petCommand("sit", 15),
			petCommand("no, bad, badgirl, badboy", 15),
			petCommand("iloveyou, loveyou", 15),
			petCommand("poop"),
			petCommand("stupid, ihateyou, dummy", 15),
			petCommand("talk, say, chat", 15),
			petCommand("sleep, sleepy, gotobed", 15),
			petCommand("change", 21),
		};

		addText("These are the commands for " .. red("Green/Red/Blue Dragon") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000033) then
	append(choices, makeChoiceHandler("What are the commands for Black Dragon?", function()
		commands = {
			petCommand("sit", 15),
			petCommand("no, bad, badgirl, badboy", 15),
			petCommand("iloveyou, loveyou", 15),
			petCommand("poop"),
			petCommand("stupid, ihateyou, dummy", 15),
			petCommand("talk, say, chat", 15),
			petCommand("sleep, sleepy, gotobed", 15),
			petCommand("cutie, change", 21),
		};

		addText("These are the commands for " .. red("Black Dragon") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000036) then
	append(choices, makeChoiceHandler("What are the commands for Jr. Reaper?", function()
		commands = {
			petCommand("sit"),
			petCommand("no, bad, badgirl, badboy"),
			petCommand("playdead, poop"),
			petCommand("talk, chat, say"),
			petCommand("iloveyou, hug"),
			petCommand("smellmyfeet, rockout, boo"),
			petCommand("trickortreat"),
			petCommand("monstermash"),
		};

		addText("These are the commands for " .. red("Jr. Reaper") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000039) then
	append(choices, makeChoiceHandler("What are the commands for Porcupine?", function()
		commands = {
			petCommand("sit"),
			petCommand("no, bad, badgirl, badboy"),
			petCommand("iloveyou, hug, goodboy"),
			petCommand("talk, chat, say"),
			petCommand("cushion, sleep, knit, poop"),
			petCommand("comb, beach", 10),
			petCommand("treeninja", 20),
			petCommand("dart", 20),
		};

		addText("These are the commands for " .. red("Porcupine") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000041) then
	append(choices, makeChoiceHandler("What are the commands for Snowman?", function()
		commands = {
			petCommand("sit"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("loveyou, mylove, ilikeyou"),
			petCommand("merrychristmas"),
			petCommand("cutie, adorable, cute, pretty"),
			petCommand("comb, beach, bad, no, badgirl, badboy"),
			petCommand("talk, chat, say, sleep, sleepy, gotobed", 10),
			petCommand("change", 20),
		};

		addText("These are the commands for " .. red("Snowman") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000045) then
	append(choices, makeChoiceHandler("What are the commands for Skunk?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("restandrelax, poop"),
			petCommand("talk, chat, say, iloveyou"),
			petCommand("snuggle, hug, sleep, goodboy"),
			petCommand("fatty, blind, badbreath", 10),
			petCommand("suitup, bringthefunk", 20),
		};

		addText("These are the commands for " .. red("Skunk") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000048) then
	append(choices, makeChoiceHandler("What are the commands for Baby Robo?", function()
		commands = {
			petCommand("sit"),
			petCommand("sleep, sleepy, gotobed"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("iloveyou, mylove, likeyou"),
			petCommand("stupid, ihateyou, dummy"),
			petCommand("fire, attack", 10),
			petCommand("talk, chat, say", 10),
			petCommand("change, go", 15),
		};

		addText("These are the commands for " .. red("Baby Robo") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000049) then
	append(choices, makeChoiceHandler("What are the commands for Blue/Red/Green/Gold Robo?", function()
		commands = {
			petCommand("sit", 15),
			petCommand("stand, up", 15),
			petCommand("stupid, ihateyou, dummy", 15),
			petCommand("bad, no, badgirl, badboy", 15),
			petCommand("hand", 15),
			petCommand("iloveyou, mylove, likeyou", 15),
			petCommand("talk, chat, say", 15),
			petCommand("fire, attack", 15),
			petCommand("sleep, sleepy, gotobed", 15),
		};

		addText("These are the commands for " .. red("Blue/Red/Green/Gold Robo") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000053) then
	append(choices, makeChoiceHandler("What are the commands for Gorilla Robo?", function()
		commands = {
			petCommand("sit", 15),
			petCommand("stupid, ihateyou, dummy", 15),
			petCommand("bad, no, badgirl, badboy", 15),
			petCommand("iloveyou, mylove, likeyou", 15),
			petCommand("fire, attack", 15),
			petCommand("sleep, sleepy, gotobed", 15),
			petCommand("talk, chat, say", 15),
			petCommand("change, go", 15),
		};

		addText("These are the commands for " .. red("Gorilla Robo") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000055) then
	append(choices, makeChoiceHandler("What are the commands for Crystal Rudolph?", function()
		commands = {
			petCommand("sit"),
			petCommand("no, badgirl, badboy, bad"),
			petCommand("bleh, joke"),
			petCommand("disguise, transform"),
			petCommand("awesome, feelgood, lalala"),
			petCommand("loveyou, heybabe"),
			petCommand("talk, chat, say", 10),
			petCommand("sleep, sleepy, nap, gotobed", 10),
		};

		addText("These are the commands for " .. red("Crystal Rudolph") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000058) then
	append(choices, makeChoiceHandler("What are the commands for White Duck?", function()
		commands = {
			petCommand("sit"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("up, sand"),
			petCommand("poop"),
			petCommand("talk, chat, say"),
			petCommand("hug"),
			petCommand("loveyou"),
			petCommand("cutie"),
			petCommand("sleep"),
			petCommand("smarty", 10),
			petCommand("dance", 20),
			petCommand("swan", 20),
		};

		addText("These are the commands for " .. red("White Duck") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000026) then
	append(choices, makeChoiceHandler("What are the commands for Sun Wu Kong?", function()
		commands = {
			petCommand("sit"),
			petCommand("poop"),
			petCommand("no, bad, badgirl, badboy"),
			petCommand("cutie, adorable, cute, pretty"),
			petCommand("iloveyou, loveyou, luvyou, ilikeyou, mylove"),
			petCommand("talk, say, chat", 10),
			petCommand("sleep, sleepy, gotobed", 10),
			petCommand("transform", 20),
		};

		addText("These are the commands for " .. red("Sun Wu Kong") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

if isValidItem(5000042) then
	append(choices, makeChoiceHandler("What are the commands for Kino?", function()
		commands = {
			petCommand("sit"),
			petCommand("poop"),
			petCommand("bad, no, badgirl, badboy"),
			petCommand("sleep, nap, sleepy, gotobed"),
			petCommand("talk, say, chat", 10),
			petCommand("iloveyou, mylove, likeyou", 10),
			petCommand("meh, bleh", 10),
			petCommand("disguise, change, transform", 20),
		};

		addText("These are the commands for " .. red("Kino") .. ".\r\n");
		addText("The level mentioned next to the command shows the pet level required for it to respond.\r\n");
		addText(commandList(commands));
		sendOk();
	end));
end

append(choices, makeChoiceHandler("Please teach me about transferring pet ability points.", function()
	if enableApTransfer then
		addText("In order to transfer the pet ability points, closeness and level, Pet AP Reset Scroll is required. ");
		addText("If you take this scroll to Mar the Fairy in Ellinia, she will transfer the level and closeness of the pet to another one. ");
		addText("I am especially giving it to you because I can feel your heart for your pet. ");
		addText("However, I can't give this out for free. ");
		addText("I can give you this book for 250,000 mesos. ");
		addText("Oh, I almost forgot! ");
		addText("Even if you have this book, it is no use if you do not have a new pet to transfer the Ability points.");
		sendNext();

		addText("250,000 mesos will be deducted. ");
		addText("Do you really want to buy?");
		answer = askYesNo();

		if answer == answer_yes then
			if getMesos() > 250000 and giveItem(4160011, 1) then
				giveMesos(-250000);
			else
				addText("Please check if your inventory has empty slot or you don't have enough mesos.");
				sendOk();
			end
		else
			-- TODO FIXME text?
		end
	else
		addText("Sorry, but this feature is not yet available.");
		sendOk();
	end
end));

addText("What do you want to know more of?\r\n\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);