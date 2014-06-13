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
-- Henesys EXP face

dofile("scripts/utils/beautyFunctions.lua");
dofile("scripts/utils/npcHelper.lua");

validFaces = getGenderStyles({
	["male"] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012, 20014},
	["female"] = {21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21012, 21014},
});

item = 5152000;

addText("If you use the regular coupon, your face may transform into a random new look...do you still want to do it using " .. blue(itemRef(item)) .. "?");
answer = askYesNo();

if answer == answer_yes then
	if giveItem(item, -1) then
		setStyle(getRandomFace(validFaces));

		addText("Enjoy!");
		sendNext();
	else
		addText("Hmm ... it looks like you don't have the coupon specifically for this place. ");
		addText("Sorry to say this, but without the coupon, there's no plastic surgery for you.");
		sendNext();
	end
else
	addText("I see ... take your time, see if you really want it. ");
	addText("Let me know when you make up your mind.");
	sendNext();
end