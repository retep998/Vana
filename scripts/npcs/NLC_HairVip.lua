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
-- Mani (VIP hair and color - NLC)

dofile("scripts/lua_functions/beautyFunctions.lua");

if getGender() == 0 then
	hairs = {30730, 30280, 30220, 30410, 30200, 30050, 30230, 30160, 30110, 30250};
elseif getGender() == 1 then
	hairs = {31730, 31310, 31470, 31150, 31160, 31300, 31260, 31220, 31410, 31270};
end

addText("I'm the head of this hair salon Mani. If you have #b#t5150031##k, #b#t5151031##k or #b#t5420001##k, allow me to take care of your hairdo. Please choose the one you want.\r\n");
addText("#b#L0# Haircut(VIP coupon)#l\r\n");
addText("#L1# Dye your hair(VIP coupon)#l\r\n");
addText("#L2# Change Hairstyle (VIP Membership Coupon)#l");
what = askChoice();

newHair = {};
if what == 0 then
	addText("I can totally change up your hairstyle and make it look so good. Why don't you change it up a bit? If you have #b#t5150031##k I'll change it for you. Choose the one to your liking~");
	getHairs(newHair, hairs);
elseif what == 1 then
	addText("I can totally change your haircolor and make it look so good. Why don't you change it up a bit? With #b#t5151026##k I'll change it for you. Choose the one to your liking.");
	getHairColours(newHair);
elseif what == 2 then
	addText("I can totally change up your hairstyle and make it look so good. Why don't you change it up a bit? If you have #b#t5420001##k I'll change it for you. With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! Now, please choose the hairstyle of your liking.");
	getHairs(newHair, hairs)
end
style = askStyle(newHair) + 1;

if what == 0 then
	item = 5150031;
elseif what == 1 then
	item = 5151026;
elseif what == 2 then
	item = 5420001;
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