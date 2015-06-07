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
-- Paul, Jean, Martin, Tony (Event Assistants)

dofile("scripts/utils/npcHelper.lua");

n = getNpcId();
addText("Hey, I'm " .. blue(npcRef(n)));
if n == 9000000 then
	addText(", if you're not busy and all ... then can I hang out with you? ");
	addText("I heard there are people gathering up around here for an " .. red("event") .. " but I don't want to go there by myself ... ");
	addText("Well, do you want to go check it out with me?");
	sendNext();

	addText("Huh? What kind of an event? ");
	addText("Well, that's... \r\n");
elseif n == 9000001 then
	addText(". I am waiting for my brother " .. blue("Paul") .. ". ");
	addText("He is supposed to be here by now...");
	sendNext();

	addText("Hmm... What should I do? ");
	addText("The event will start, soon... ");
	addText("Many people went to participate in the event, so we better be hurry...");
	sendBackNext();

	addText("hey... Why don't you go with me? ");
	addText("I think my brother will come with other people.\r\n");
elseif n == 9000011 then
	addText(". I am waiting for my brothers... ");
	addText("What takes them so long? ");
	addText("I got bored now... ");
	addText("If we do not get there on time, we might not be able to participate in the event...");
	sendNext();

	addText("Hmm... What should I do? ");
	addText("The event will start, soon... ");
	addText("Many people went to participate in the event, so we better be hurry...");
	sendBackNext();

	addText("Hey... why don't you go with me, then?\r\n");
elseif n == 9000013 then
	addText(". I've been waiting for my brothers for a while, but they haven't gotten here yet. ");
	addText("I'm sick and tired of doing things by myself. ");
	addText("At least during the event, I don't feel quite so lonely with so many people around me and all. ");
	addText("All events require a limited number of people, so if I don't get there fast enough, I won't be able to join.");
	sendNext();

	addText("We may be cousins and all, but we keep on missing each other. ");
	addText("Man, what should I do? ");
	addText("The event should start any minute ... ");
	addText("A lot of people should be there waiting, and there may not be any space for them ...");
	sendBackNext();

	addText("What do you think? ");
	addText("Do you want to join me and head over to the event?\r\n");
else
	sendNext();
end

choices = {
	makeChoiceHandler(bold("1. ") .. blue("What kind of an event is it?"), function()
		addText("All this month MapleStory Global is celebrating its 1st anniversary! ");
		addText("The GM's will be holding surprise GM Events throughout the event, so stay on your toes and make sure to participate in at least one of the events for great prizes!");
		sendNext();
	end),
	makeChoiceHandler(bold("2. ") .. blue("Explain the event game to me."), function()
		gameChoices = {
			makeChoiceHandler(" Ola Ola", function()
				addText(blue("[Ola Ola]") .. " is a game where participants climb ladders to reach the top. ");
				addText("Climb your way up and move to the next level by choosing the correct portal out of the numerous portals available. \r\n");
				addText("The game consists of three levels, and the time limit is " .. blue("6 MINUTES") .. ". ");
				addText("During [Ola Ola], you " .. blue("won't be able to jump, teleport, haste, or boost your speed using potions or items") .. ". ");
				addText("There are also trick portals that'll lead you to a strange place, so please be aware of those.");
				sendNext();
			end),
			makeChoiceHandler(" MapleStory Maple Physical Fitness Test", function()
				addText(blue("[MapleStory Physical Fitness Test] is a race through an obstacle course") .. " much like the Forest of Patience. ");
				addText("You can win it by overcoming various obstacles and reach the final destination within the time limit.\r\n");
				addText("The game consists of four levels, and the time limit is " .. blue("15 MINUTES") .. ". ");
				addText("During [MapleStory Physical Fitness Test], you won't be able to use teleport or haste.");
				sendNext();
			end),
			makeChoiceHandler(" Snow Ball", function()
				addText(blue("[Snowball]") .. " consists of two teams, Maple Team and Story Team, and the two teams duke it out to see " .. blue("which team rolled the snowball farther and bigger in a limited time") .. ". ");
				addText("If the game cannot be decided within the time period, then the team that rolled the snowball farther wins.\r\n");
				addText("To roll up the snow, attack it by pressing Ctrl. ");
				addText("All long-range attacks and skill-based attack will not work here; " .. blue("only the close-range attacks will work") .. ".\r\n");
				addText("If a character touches the snowball, he/she'll be sent back to the starting point. ");
				addText("Attack the snowman in front of the starting point to prevent the opposing team from rolling the snow forward. ");
				addText("This is where a well-planned strategy works, as the team will decide whether to attack the snowball or the snowman.");
				sendNext();
			end),
			makeChoiceHandler(" Coconut Harvest", function()
				addText(blue("[Coconut Harvest]") .. " consists of two teams, Maple Team and Story Team, and the two teams duke it out to see " .. blue("which team gathers up more coconuts") .. ". ");
				addText("The time limit is " .. blue("5 MINUTES") .. ". ");
				addText("If the game ends in a tie, an additional 2 minutes will be awarded to determine the winner. ");
				addText("If, for some reason, the score stays tied, then the game will end in a draw.\r\n");
				addText("All long-range attacks and skill-based attacks will not work here; " .. blue("only the close-range attacks will work") .. ". ");
				addText("If you don't have a weapon for the close-range attacks, you can purchase them through an NPC within the event map. ");
				addText("No matter the level of character, the weapon, or skills, all damages applied will be the same.\r\n");
				addText("Beware of the obstacles and traps within the map. ");
				addText("If the character dies during the game, the character will be eliminated from the game. ");
				addText("The player who strikes last before the coconut drops wins. ");
				addText("Only the coconuts that hit the ground counts, which means the ones that do not fall off the tree, or the occasional explosion of the coconuts WILL NOT COUNT. ");
				addText("There's also a hidden portal at one of the shells at the bottom of the map, so use that wisely!");
				sendNext();
			end),
			makeChoiceHandler(" OX Quiz", function()
				addText(blue("[OX Quiz]") .. " is a game of MapleStory smarts through X's and O's. ");
				addText("Once you join the game, turn on the minimap by pressing M to see where the X and O are. ");
				addText("A total of " .. red("10 questions") .. " will be given, and the character that answers them all correctly wins the game.\r\n");
				addText("Once the question is given, use the ladder to enter the area where the correct answer may be, be it X or O. ");
				addText("If the character does not choose an answer or is hanging on the ladder past the time limit, the character will be eliminated. ");
				addText("Please hold your position until [CORRECT] is off the screen before moving on. ");
				addText("To prevent cheating of any kind, all types of chatting will be turned off during the OX quiz.");
				sendNext();
			end),
			makeChoiceHandler(" Treasure Hunt", function()
				addText(blue("[Treasure Hunt]") .. " is a game in which your goal is to find the " .. blue("treasure scrolls") .. " that are hidden all over the map " .. red("in 10 minutes") .. ". ");
				addText("There will be a number of mysterious treasure chests hidden away, and once you break them apart, many items will surface from the chest. ");
				addText("Your job is to pick out the treasure scroll from those items.\r\n");
				addText("Treasure chests can be destroyed using " .. blue("regular attacks") .. ", and once you have the treasure scroll in possession, you can trade it for the Scroll of Secrets through an NPC that's in charge of trading items. ");
				addText("The trading NPC can be found on the Treasure Hunt map, but you can also trade your scroll through " .. blue("[Vikin]") .. " of Lith Harbor.\r\n");
				addText("This game has its share of hidden portals and hidden teleporting spots. ");
				addText("To use them, press the " .. blue("up arrow") .. " at a certain spot, and you'll be teleported to a different place. ");
				addText("Try jumping around, for you may also run into hidden stairs or ropes. ");
				addText("There will also be a treasure chest that'll take you to a hidden spot, and a hidden chest that can only be found through the hidden portal, so try looking around.\r\n");
				addText("During the game of Treasure Hunt, all attack skills will be " .. red("disabled") .. ", so please break the treasure chest with the regular attack.");
				sendNext();
			end),
		};

		addText("There are many games for this event. ");
		addText("It will help you a lot to know how to play the game before you play it. ");
		addText("Choose the one you want to know more of!\r\n");
		addText(blue(choiceRef(gameChoices)));
		choice = askChoice();

		selectChoice(gameChoices, choice);
	end),
	makeChoiceHandler(bold("3. ") .. blue("Alright, let's go!"), function()
		-- TODO FIXME implement events
		addText("Either the event has not been started, you already have " .. itemRef(4031019) .. ", or have already participated in this event within the last 24 hours. ");
		addText("Please try again later!");
		sendNext();
	end),
};

addText(choiceRef(choices));
choice = askChoice();

selectChoice(choices, choice);