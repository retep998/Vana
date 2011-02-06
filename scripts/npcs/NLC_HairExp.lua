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
-- Ari (EXP hair and color - NLC)

dofile("scripts/lua_functions/beautyFunctions.lua");

if getGender() == 0 then
	hairs = {30400, 30360, 30440, 30410, 30200, 30050, 30230, 30160, 30110, 30250};
elseif getGender() == 1 then
	hairs = {31560, 31720, 31450, 31150, 31160, 31300, 31260, 31220, 31410, 31270};
end

addText("I'm Brittany the assistant. If you have #b#t5150030##k or #b#t5151025##k by any chance, then how about letting me change your hairdo?\r\n");
addText("#b#L0# Haircut(EXP coupon)#l\r\n");
addText("#L2# Dye your hair(REG coupon)#l");
what = askChoice();

if what == 0 then
	item = 5150030;
	addText("If you use the EXP coupon your hair will change RANDOMLY with a chance to obtain a new experimental style that even you didn't think was possible. Are you going to use #b#t5150030##k and really change your hairstyle?");
elseif what == 2 then
	item = 5151025;
	addText("If you use a regular coupon your hair will change RANDOMLY. Do you still want to use #b#t5151025##k and change it up?");
end
yes = askYesNo();

if yes == 1 then
	if getItemAmount(item) > 0 then
		giveItem(item, -1);
		giveRandomHair(hairs);
		addText("Enjoy!");
		sendOK();
	else
		addText("Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry.");
		sendNext();
	end
else
	addText("I understand...think about it, and if you still feel like changing come talk to me.");
	sendNext();
end
