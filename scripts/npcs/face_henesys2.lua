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
-- Henesys EXP eyes

if getGender() == 0 then
	face = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012, 20014};
else
	face = {21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21012, 21014};
end

addText("If you use the regular coupon, your face may transform into a random new look...do you still want to do it using #b#t5152000##k?");
yes = askYesNo();

if yes == 1 then
	if getItemAmount(5152000) > 0 then
		giveItem(5152000, -1);
		setStyle(face[getRandomNumber(#face)]);
		addText("Enjoy!");
		sendNext();
	else
		addText("Hmm ... it looks like you don't have the coupon specifically for this place. Sorry to say this, but without the coupon, there's no plastic surgery for you.");
		sendNext();
	end
else
	addText("I see ... take your time, see if you really want it. Let me know when you make up your mind.");
	sendNext();
end