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
-- Natalie (VIP hair and color - Henesys)

dofile("scripts/lua_functions/beautyFunctions.lua");

if getGender() == 0 then
	hairs = {30030, 30020, 30000, 30310, 30330, 30060, 30150, 30410, 30210, 30140, 30120, 30200};
elseif getGender() == 1 then
	hairs = {31050, 31040, 31000, 31150, 31310, 31300, 31160, 31100, 31410, 31030, 31080, 31070};
end

addText("I'm the head of this hair salon Natalie. If you have #b#t5150001##k, #b#t5151001##k or #b#t5420002##k, allow me to take care of your hairdo. Please choose the one you want.\r\n");
addText("#b#L0# Haircut(VIP coupon)#l\r\n");
addText("#L1# Dye your hair(VIP coupon)#l\r\n");
addText("#L2# Change Hairstyle (VIP Membership Coupon)#l");
what = askChoice();

newHair = {};
if what == 0 then
	addText("I can totally change up your hairstyle and make it look so good. Why don't you change it up a bit? If you have #b#t5150001##k I'll change it for you. Choose the one to your liking~");
	getHairs(newHair, hairs);
elseif what == 1 then
	addText("I can totally change your haircolor and make it look so good. Why don't you change it up a bit? With #b#t51051001##k I'll change it for you. Choose the one to your liking.");
	getHairColours(newHair);
elseif what == 2 then
	addText("I can totally change up your hairstyle and make it look so good. Why don't you change it up a bit? If you have #b#t5420002##k I'll change it for you. With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! Now, please choose the hairstyle of your liking.");
	getHairs(newHair, hairs)
end
style = askStyle(newHair) + 1;

if what == 0 then
	item = 5150001;
elseif what == 1 then
	item = 5151001;
elseif what == 2 then
	item = 5420002;
end
if getItemAmount(item) > 0 then
	giveItem(item, -1);
	if newHair[style] > 0  then
		setStyle(newHair[style]);
	end
	addText("Enjoy your new and improved hairstyle!");
	sendOK();
else
	addText("Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't ");
	if what == 0 or what == 2 then
		addText("give you a haircut ");
	elseif what == 1 then
		addText("dye your hair ");
	end
	addText("without it. I'm sorry.");
	sendNext();
end