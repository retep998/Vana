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
-- NLC VIP eyes

dofile("scripts/utils/beautyFunctions.lua");
dofile("scripts/utils/npcHelper.lua");

choices = getFaceStyles(getGenderStyles({
	["male"] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012},
	["female"] = {21001, 21002, 21003, 21004, 21005, 21006, 21008, 21012, 21016},
}));

itemId = 5152034;

addText("Let's see... ");
addText("I can totally transform your face into something new. ");
addText("Don't you want to try it? ");
addText("For " .. blue(itemRef(itemId)) .. ", you can get the face of your liking. ");
addText("Take your time in choosing the face of your preference...");
choice = askStyle(choices);

if giveItem(itemId, -1) then
	setStyle(selectChoice(choices, choice));
	addText("Enjoy your new and improved face!");
	sendNext();
else
	addText("Hmm ... it looks like you don't have the coupon specifically for this place...sorry to say this, but without the coupon, there's no plastic surgery for you.");
	sendNext();
end