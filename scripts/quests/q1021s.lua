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
-- Roger: Apple Quest start

dofile("scripts/utils/npcHelper.lua");

if getGender() == gender_male then
	addText("Hey, Man~  ");
	addText("What's up? ");
	addText("Haha!  ");
	addText("I am " .. npcRef(2000) .. " who can teach you adorable new Maplers lots of information.");
	sendNext();

	addText("You are asking who made me do this?  ");
	addText("Ahahahaha!  ");
	addText("Myself!  ");
	addText("I wanted to do this and just be kind to you new travellers.");
	sendBackNext();
else
	addText("Hey there, Pretty~ ");
	addText("I am " .. npcRef(2000) .. " who teaches you adorable new Maplers lots of information.");
	sendNext();

	addText("I know you are busy! ");
	addText("Please spare me some time~ I can teach you some useful information! ");
	addText("Ahahaha!");
	sendBackNext();
end

addText("So.....  Let me just do this for fun! ");
addText("Abaracadabra~!");
answer = askAcceptDecline();

if answer == answer_accept then
	setHp(getHp() / 2);
	giveItem(2010007, 1);
	addQuest(1021);
	addText("Surprised?  ");
	addText("If HP becomes 0, then you are in trouble.  ");
	addText("Now, I will give you " .. red(itemRef(2010007)) .. ".  ");
	addText("Please take it.  ");
	addText("You will feel stronger.  ");
	addText("Open the Item window and double click to consume.  ");
	addText("Hey, It's very simple to open the Item window.  ");
	addText("Just press " .. blue("I") .. " on your keyboard.#I");
	sendNext();

	addText("Please take all " .. itemRef(2010007) .. "s that I gave you.  ");
	addText("You will be able to see the HP bar increasing.  ");
	addText("Please talk to me again when you recover your HP 100%#I ");
	sendBackOk();
else
	if getGender() == gender_male then
		addText("I can't believe an attractive guy like myself, got turned down!");
		sendNext();
	else
		addText("I can't believe you have just turned down an attractive guy like me!");
		sendNext();
	end
end