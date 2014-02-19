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
-- Brittany (EXP hair and color - Henesys)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

if getGender() == gender_male then
	hairs = {30310, 30330, 30060, 30150, 30410, 30210, 30140, 30120, 30200, 30560, 30510, 30610, 30470};
elseif getGender() == gender_female then
	hairs = {31150, 31310, 31300, 31160, 31100, 31410, 31030, 31080, 31070, 31610, 31350, 31510, 31740};
end

addText("I'm Brittany the assistant. ");
addText("If you have " .. blue(itemRef(5150010)) .. " or " .. blue(itemRef(5150000)) .. " by any chance, then how about letting me change your hairdo?\r\n");
addText(blue(choiceList({
	" Haircut(EXP coupon)",
	" Dye your hair(REG coupon)",
})));
choice = askChoice();

if choice == 0 then
	item = 5150010;
	addText("If you use the EXP coupon your hair will change RANDOMLY with a chance to obtain a new experimental style that even you didn't think was possible. ");
	addText("Are you going to use " .. blue(itemRef(5150010)) .. " and really change your hairstyle?");
elseif choice == 2 then
	item = 5151000;
	addText("If you use a regular coupon your hair will change RANDOMLY. ");
	addText("Do you still want to use " .. blue(itemRef(5150000)) .. " and change it up?");
end
answer = askYesNo();

if answer == answer_yes then
	if getItemAmount(item) > 0 then
		giveItem(item, -1);
		giveRandomHair(hairs);
		addText("Enjoy!");
		sendOk();
	else
		addText("Hmmm...it looks like you don't have our designated coupon...");
		addText("I'm afraid I can't give you a haircut without it. ");
		addText("I'm sorry.");
		sendNext();
	end
else
	addText("I understand...think about it, and if you still feel like changing come talk to me.");
	sendNext();
end