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
-- Denma the owner (Henesys VIP face)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

validFaces = getGenderStyles({
	["male"] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012, 20014},
	["female"] = {21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21012, 21014},
});

item = 5152001;

validFaces = getFaceStyles(validFaces)

addText("Let's see... ");
addText("I can totally transform your face into something new. ");
addText("Don't you want to try it? ");
addText("For " .. blue(itemRef(item)) .. ", you can get the face of your liking. ");
addText("Take your time in choosing the face of your preference...");
choice = askStyle(validFaces);

if giveItem(item, -1) then
	setStyle(selectChoice(validFaces, choice));

	addText("Enjoy your new and improved face!");
	sendNext();
else
	addText("Hmm ... it looks like you don't have the coupon specifically for this place...sorry to say this, but without the coupon, there's no plastic surgery for you.");
	sendNext();
end