--[[
Copyright (C) 2008-2011 Vana Development Team

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

n = getNPCID();
addText("Hey, I'm #b#p" .. n .. "##k");
if n == 9000000 then
	addText(", if you're not busy and all ... then can I hang out with you? I heard there are people gathering up around here for an #revent#k but I don't want to go there by myself ... Well, do you want to go check it out with me?");
	sendNext();

	addText("Huh? What kind of an event? Well, that's... \r\n");
elseif n == 9000001 then
	addText(". I am waiting for my brother #bPaul#k. He is supposed to be here by now...");
	sendNext();

	addText("Hmm... What should I do? The event will start, soon... Many people went to participate in the event, so we better be hurry...");
	sendBackNext();

	addText("hey... Why don't you go with me? I think my brother will come with other people.\r\n");
elseif n == 9000011 then
	addText(". I am waiting for my brothers... What takes them so long? I got bored now... If we do not get there on time, we might not be able to participate in the event...");
	sendNext();

	addText("Hmm... What should I do? The event will start, soon... Many people went to participate in the event, so we better be hurry...");
	sendBackNext();

	addText("Hey... why don't you go with me, then?\r\n");
elseif n == 9000013 then
	addText(". I've been waiting for my brothers for a while, but they haven't gotten here yet. I'm sick and tired of doing things by myself. At least during the event, I don't feel quite so lonely with so many people around me and all. All events require a limited number of people, so if I don't get there fast enough, I won't be able to join.");
	sendNext();

	addText("We may be cousins and all, but we keep on missing each other. Man, what should I do? The event should start any minute ... A lot of people should be there waiting, and there may not be any space for them ...");
	sendBackNext();

	addText("What do you think? Do you want to join me and head over to the event?\r\n");
else
	sendNext();
end

addText("#L0##e1. #n#bWhat kind of an event is it?#k#l\r\n");
addText("#L1##e2. #n#bExplain the event game to me.#k#l\r\n");
addText("#L2##e3. #n#bAlright, let's go!#k#l");
option = askChoice();

if option == 0 then
	addText("All this month MapleStory Global is celebrating its 1st anniversary! The GM's will be holding surprise GM Events throughout the event, so stay on your toes and make sure to participate in at least one of the events for great prizes!");
	sendNext();
elseif option == 1 then
	addText("There are many games for this event. It will help you a lot to know how to play the game before you play it. Choose the one you want to know more of!\r\n");
	addText("#b#L0# Ola Ola#l\r\n");
	addText("#L1# MapleStory Maple Physical Fitness Test#l\r\n");
	addText("#L2# Snow Ball#l\r\n");
	addText("#L3# Coconut Harvest#l\r\n");
	addText("#L4# OX Quiz#l\r\n");
	addText("#L5# Treasure Hunt#l#k");
	info = askChoice();

	if info == 0 then
		addText("#b[Ola Ola]#k is a game where participants climb ladders to reach the top. Climb your way up and move to the next level by choosing the correct portal out of the numerous portals available. \r\n");
		addText("The game consists of three levels, and the time limit is #b6 MINUTES#k. During [Ola Ola], you #bwon't be able to jump, teleport, haste, or boost your speed using potions or items#k. There are also trick portals that'll lead you to a strange place, so please be aware of those.");
	elseif info == 1 then
		addText("#b[MapleStory Physical Fitness Test] is a race through an obstacle course#k much like the Forest of Patience. You can win it by overcoming various obstacles and reach the final destination within the time limit.\r\n");
		addText("The game consists of four levels, and the time limit is #b15 MINUTES#k. During [MapleStory Physical Fitness Test], you won't be able to use teleport or haste.");
	elseif info == 2 then
		addText("#b[Snowball]#k consists of two teams, Maple Team and Story Team, and the two teams duke it out to see #bwhich team rolled the snowball farther and bigger in a limited time#k. If the game cannot be decided within the time period, then the team that rolled the snowball farther wins.\r\n");
		addText("To roll up the snow, attack it by pressing Ctrl. All long-range attacks and skill-based attack will not work here; #bonly the close-range attacks will work#k.\r\n");
		addText("If a character touches the snowball, he/she'll be sent back to the starting point. Attack the snowman in front of the starting point to prevent the opposing team from rolling the snow forward. This is where a well-planned strategy works, as the team will decide whether to attack the snowball or the snowman.");
	elseif info == 3 then
		addText("#b[Coconut Harvest]#k consists of two teams, Maple Team and Story Team, and the two teams duke it out to see #bwhich team gathers up more coconuts#k. The time limit is #b5 MINUTES#k. If the game ends in a tie, an additional 2 minutes will be awarded to determine the winner. If, for some reason, the score stays tied, then the game will end in a draw.\r\n");
		addText("All long-range attacks and skill-based attacks will not work here; #bonly the close-range attacks will work#k. If you don't have a weapon for the close-range attacks, you can purchase them through an NPC within the event map. No matter the level of character, the weapon, or skills, all damages applied will be the same.\r\n");
		addText("Beware of the obstacles and traps within the map. If the character dies during the game, the character will be eliminated from the game. The player who strikes last before the coconut drops wins. Only the coconuts that hit the ground counts, which means the ones that do not fall off the tree, or the occasional explosion of the coconuts WILL NOT COUNT. There's also a hidden portal at one of the shells at the bottom of the map, so use that wisely!");
	elseif info == 4 then
		addText("#b[OX Quiz]#k is a game of MapleStory smarts through X's and O's. Once you join the game, turn on the minimap by pressing M to see where the X and O are. A total of #r10 questions#k will be given, and the character that answers them all correctly wins the game.\r\n");
		addText("Once the question is given, use the ladder to enter the area where the correct answer may be, be it X or O. If the character does not choose an answer or is hanging on the ladder past the time limit, the character will be eliminated. Please hold your position until [CORRECT] is off the screen before moving on. To prevent cheating of any kind, all types of chatting will be turned off during the OX quiz.");
	elseif info == 5 then
		addText("#b[Treasure Hunt]#k is a game in which your goal is to find the #btreasure scrolls#k that are hidden all over the map #rin 10 minutes#k. There will be a number of mysterious treasure chests hidden away, and once you break them apart, many items will surface from the chest. Your job is to pick out the treasure scroll from those items.\r\n");
		addText("Treasure chests can be destroyed using #bregular attacks#k, and once you have the treasure scroll in possession, you can trade it for the Scroll of Secrets through an NPC that's in charge of trading items. The trading NPC can be found on the Treasure Hunt map, but you can also trade your scroll through #b[Vikin]#k of Lith Harbor.\r\n");
		addText("This game has its share of hidden portals and hidden teleporting spots. To use them, press the #bup arrow#k at a certain spot, and you'll be teleported to a different place. Try jumping around, for you may also run into hidden stairs or ropes. There will also be a treasure chest that'll take you to a hidden spot, and a hidden chest that can only be found through the hidden portal, so try looking around.\r\n");
		addText("During the game of Treasure Hunt, all attack skills will be #rdisabled#k, so please break the treasure chest with the regular attack.");
	end
	sendNext();
elseif option == 2 then
	-- TODO: Add event code
	addText("Either the event has not been started, you already have #t4031019#, or have already participated in this event within the last 24 hours. Please try again later!");
	sendNext();
end