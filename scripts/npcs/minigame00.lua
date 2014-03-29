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
-- Casey of Henesys, Chico of Ludi

dofile("scripts/lua_functions/npcHelper.lua");

isGameZone = getNpcId() == 1012008;
isLudi = getNpcId() == 2040014;

choices = {
	makeChoiceHandler("Create a minigame item", function()
		choices = {
			makeChoiceHandler("Omok Set", function()
				function makeSetOption(setId, req1, req2)
					return makeChoiceData(itemRef(setId), {setId, req1, req2})
				end

				if isGameZone then
					choices = {
						makeSetOption(4080000, 4030000, 4030001),
						makeSetOption(4080001, 4030000, 4030010),
						makeSetOption(4080002, 4030000, 4030011),
						makeSetOption(4080003, 4030010, 4030001),
						makeSetOption(4080004, 4030011, 4030010),
						makeSetOption(4080005, 4030011, 4030001),
					};
				elseif isLudi then
					choices = {
						makeSetOption(4080006, 4030013, 4030014),
						makeSetOption(4080007, 4030013, 4030016),
						makeSetOption(4080008, 4030014, 4030016),
						makeSetOption(4080009, 4030015, 4030013),
						makeSetOption(4080010, 4030015, 4030014),
						makeSetOption(4080011, 4030015, 4030016),
					};
				end

				addText("You want to play " .. blue("Omok") .. ", huh? ");
				addText("To play it, you'll need the Omok Set. ");
				addText("Only the ones with that item can open the room for a game of Omok, and you can play this game almost anywhere except for a few places at the market place.");
				sendNext();

				addText("The set also differs based on what kind of pieces you want to use for the game. ");
				addText("Which set would you like to make?\r\n");
				addText(blue(choiceList(choices)));
				choice = askChoice();

				data = selectChoice(choices, choice);
				setId, req1, req2 = data[1], data[2], data[3];
				boardId = 4030009;

				addText(blue("You want to make " .. itemRef(setId)) .. "? ");
				addText("Hmm... get me the materials, and I can do just that. ");
				addText("Listen carefully, the materials you need will be: ");
				addText(red("99 " .. itemRef(req1) .. ", 99 " .. itemRef(req2) .. ", 1 " .. itemRef(boardId)) .. ". ");
				addText("The monsters will probably drop those every once in a while...");
				sendNext();

				if getItemAmount(req1) >= 99 and getItemAmount(req2) >= 99 and getItemAmount(boardId) >= 1 then
					addText("Wow, you managed to obtain " .. red(itemRef(req1) .. ", " .. itemRef(req2) .. ", " .. itemRef(boardId)) .. "! ");
					addText("Well, you brought all the necessary items. ");
					addText("Okay, wait a second for me to make it.");
					sendNext();

					if hasOpenSlotsFor(setId, 1) then
						giveItem(req1, -99);
						giveItem(req2, -99);
						giveItem(boardId, -1);
						giveItem(setId, 1);

						addText("Here you are, " .. blue(itemRef(setId)) .. "! ");
						addText("You can open an Omok room anywhere in the game and have fun playing against other Maple users. ");
						addText("Open a room here and play with many other users. ");
						addText("If you finish with an admirable record, something good can happen. ");
						addText("I'll be rooting for you, go play!");
						sendNext();

						addText("Oh, if you have any questions about the game of Omok, feel free to ask. ");
						addText("I'll be here for a while. ");
						addText("Well, practice playing with others until you feel you can defeat me in minigames. ");
						addText("But of course, that won't happen, haha... ");
						addText("Okay, I'm off~");
						sendNext();
					else
						addText("Are you sure you have " .. blue(itemRef(req1) .. ", " .. itemRef(req2) .. ", " .. itemRef(boardId)) .. "? ");
						addText("If so, make sure your etc. inventory has space.");
						sendNext();
					end
				else
					-- Intentionally left blank
				end
			end),
			makeChoiceHandler("A Set of Match Cards", function()
				addText("You want " .. blue(itemRef(4080100)) .. "? ");
				addText("Hmmm... to make " .. itemRef(4080100) .. ", you'll need some " .. blue(itemRef(4030012) .. "s") .. ". ");
				addText(itemRef(4030012) .. " can be obtained by taking out the monsters all around the island. ");
				addText("Collect 99 " .. itemRef(4030012) .. "s and you can make a set of Match Cards.");
				sendNext();

				if getItemAmount(4030012) >= 99 then
					addText("Wow, you managed to obtain " .. red("99 " .. itemRef(4030012)) .. "!! ");
					addText("Great... okay, this is going to be fun. ");
					addText("Wait a second~ ");
					addText("I'll make " .. red(itemRef(4080100)) .. " quickly. ");
					sendNext();

					if hasOpenSlotsFor(4080100, 1) then
						giveItem(4030012, -99);
						giveItem(4080100, 1);

						addText("Here you are, " .. blue(itemRef(4080100)) .. "! ");
						addText("The \"Match Cards\" room may be played almost everywhere in the game. ");
						addText("Open a room here and play with many other users. ");
						addText("If you finish with a great record, something good can happen. ");
						addText("I'll be rooting for you, go play!");
						sendNext();

						addText("Oh, if you have any questions about the game of Match Cards, feel free to ask. ");
						addText("I'll be here for a while. ");
						addText("Well, practice playing with others until you feel you can defeat me in minigames. ");
						addText("But of course, that won't happen, haha... ");
						addText("Okay, I'm off~");
						sendNext();
					else
						addText("Are you sure you have " .. red("99 " .. itemRef(4080100) .. "s") .. "? ");
						addText("If so, make sure your etc. inventory has space.");
						sendNext();
					end
				else
					-- Intentionally left blank
				end
			end),
		};

		addText("You want to make the minigame item? ");
		addText("Minigames aren't something you can just go ahead and play right off the bat. ");
		addText("You'll need a specific set of items for a specific minigame. ");
		addText("Which minigame item do you want to make?\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end),
	makeChoiceHandler("Explain to me what the minigames are about", function()
		choices = {
			makeChoiceHandler("Omok", function()
				addText("Here are the rules to the game of Omok. ");
				addText("Listen carefully. ");
				addText("Omok is a game where, you and your opponent will take turns laying a piece on the table until someone finds a way to lay 5 consecutive pieces in a line, be it horizontal, diagonal or vertical. ");
				addText("That person will be the winner. ");
				addText("For starters, only the ones with " .. blue("Omok Set") .. " can open a game room.");
				sendNext();

				addText("Every game of Omok will cost you " .. red("100 mesos") .. ". ");
				addText("Even if you don't have " .. blue("Omok set") .. ", you can enter the game room and play the game. ");
				addText("If you don't have 100 mesos, however, then you won't be allowed in the room, period. ");
				addText("The person opening the game room also needs 100 mesos to open the room, or there's no game. ");
				addText("If you run out of mesos during the game, then you're automatically kicked out of the room!");
				sendNext();

				addText("Enter the room, and when you're ready to play, click on " .. blue("Ready") .. ". ");
				addText("Once the visitor clicks on " .. blue("Ready") .. ", the owner of the room can press " .. blue("Start") .. " to start the game. ");
				addText("If an unwanted visitor walks in, and you don't want to play with that person, the owner of the room has the right to kick the visitor out of the room. ");
				addText("There will be a square box with x written on the right of that person. ");
				addText("Click on that for a cold goodbye, ok?");
				sendNext();

				addText("When the first game starts, " .. blue("the owner of the room goes first") .. ". ");
				addText("Be ware that you'll be given a time limit, and you may lose your turn if you don't make your move on time. ");
				addText("Normally, 3 x 3 is not allowed, but if there comes a point that it's absolutely necessary to put your piece there or face a game over, then you can put it there. ");
				addText("3 x 3 is allowed as the last line of defense! ");
				addText("Oh, and it won't count if it's " .. red("6 or 7 straight") .. ". ");
				addText("Only 5!");
				sendNext();

				addText("If you know your back is up against the wall, you can request a " .. blue("Redo") .. ". ");
				addText("If the opponent accepts your request, then the opponent's last move, along with yours, will be canceled out. ");
				addText("If you ever feel the need to go to the bathroom, or take an extended break, you can request a " .. blue("tie") .. ". ");
				addText("The game will end in a tie if the opponent accepts the request. ");
				addText("This may be a good way to keep your friendship intact with your buddy.");
				sendNext();

				addText("Once the game is over, and the next game starts, the loser will go first. ");
				addText("Oh, and you can't leave in the middle of the game. ");
				addText("If you do, you may need to request either a " .. blue("forfeit") .. ", or a " .. blue("tie") .. ". ");
				addText("Of course, if you request a forfeit, you'll lose the game, so be careful of that. ");
				addText("And if you click on \"Leave\" in the middle of the game and call to leave after the game, you'll leave the room right after the game is over, so this will be a much more useful way to leave.");
				sendNext();
			end),
			makeChoiceHandler("Match Cards", function()
				addText("Here are the rules to the game of Match Cards. ");
				addText("Listen carefully. ");
				addText("Match Cards is just like the way it sounds, finding a matching pair among the number of cards laid on the table. ");
				addText("When all the matching pairs are found, then the person with more matching pairs will win the game. ");
				addText("Just like Omok, you'll need " .. blue(itemRef(4080100)) .. " to open the game room.");
				sendNext();

				addText("Every game of Match Cards will cost you " .. red("100 mesos") .. ". ");
				addText("Even if you don't have " .. blue(itemRef(4080100)) .. ", you can enter the game room and play the game. ");
				addText("If you don't have 100 mesos, however, then you won't be allowed in the room, period. ");
				addText("The person opening the game room also needs 100 mesos to open the room, or there's no game. ");
				addText("If you run out of mesos during the game, then you're automatically kicked out of the room!");
				sendNext();

				addText("Enter the room, and when you're ready to play, click on " .. blue("Ready") .. ". ");
				addText("Once the visitor clicks on " .. blue("Ready") .. ", the owner of the room can press " .. blue("Start") .. " to start the game. ");
				addText("If an unwanted visitor walks in, and you don't want to play with that person, the owner of the room has the right to kick the visitor out of the room. ");
				addText("There will be a square box with x written on the right of that person. ");
				addText("Click on that for a cold goodbye, ok?");
				sendNext();

				addText("Oh, and unlike Omok, on Match Cards, when you create the game room, you'll need to set your game on the number of cards you'll use for the game. ");
				addText("There are three modes available: 3x4, 4x5, and 5x6, which will require 12, 20, and 30 cards. ");
				addText("Remember, though, you won't be able to change it up once the game room is open, so if you really wish to change it up, you may have to close the room and open another one.");
				sendNext();

				addText("When the first game starts, " .. blue("the owner of the room goes first") .. ". ");
				addText("Beware that you'll be given a time limit, and you may lose your turn if you don't make your move on time. ");
				addText("When you find a matching pair on your turn, you'll get to keep your turn, as long as you keep finding a pair of matching cards. ");
				addText("Use your memorizing skills for a devestating combo of turns.");
				sendNext();

				addText("If you and your opponent have the same number of matched pairs, then whoever had a longer streak of matched pairs will win. ");
				addText("If you ever feel the need to go to the bathroom, or take an extended break, you can request a " .. blue("tie") .. ". ");
				addText("The game will end in a tie if the opponent accepts the request. ");
				addText("This may be a good way to keep your friendship intact with your buddy.");
				sendNext();

				addText("Once the game is over, and the next game starts, the loser will go first. ");
				addText("Oh, and you can't leave in the middle of the game. ");
				addText("If you do, you may need to request either a " .. blue("forfeit") .. ", or a " .. blue("tie") .. ". ");
				addText("Of course, if you request a forfeit, you'll lose the game, so be careful of that. ");
				addText("And if you click on \"Leave\" in the middle of the game and call to leave after the game, you'll leave the room right after the game is over, so this will be a much more useful way to leave.");
				sendOk();
			end),
		};

		addText("You want to learn more about the minigames? ");
		addText("Awesome! ");
		addText("Ask me anything. ");
		addText("Which minigame do you want to know more about?\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end),
};

if isGameZone then
	addText("Hey, you look like you need a breather from all that hunting. ");
	addText("You should be enjoying the life, just like I am. ");
	addText("Well, if you have a couple of items, I can make a trade with you for an item you can play minigames with. ");
	addText("Now... what can I do for you?\r\n");
elseif isLudi then
	addText("Well, hello! ");
	addText("I'm " .. blue(npcRef(2040014)) .. " and I'm responsible for everything involving mini-games. ");
	addText("Looks like you have an interest in mini-games... ");
	addText("I can certainly help you! ");
	addText("Okay... so what can I do for you?\r\n");
else
	consoleOutput("Unsupported minigame00 NPC/map: " .. getNpcId() .. "/" .. getMap());
	return;
end
addText(blue(choiceList(choices)));
choice = askChoice();

selectChoice(choices, choice);