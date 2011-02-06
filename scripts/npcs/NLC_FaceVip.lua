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
-- NLC VIP eyes

if getGender() == 0 then
	face = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20008, 20023};
else
	face = {21001, 21002, 21003, 21004, 21005, 21006, 21008, 21012, 21022};
end

-- Get player's eyes to get the face
eye = getEyes();

addText("Let's see... I can totally transform your face into something new. Don't you want to try it? For #b#t5152034##k, you can get the face of your liking. Take your time in choosing the face of your preference...");
current = eye - (eye % 10000);
newface = {};
for i = 0, #face do
	if not (current + i == eye) then
		newface[#newface + 1] = current + i;
	end
end
style = askStyle(newface) + 1;

if getItemAmount(5152034) > 0 then
	giveItem(5152034, -1);
	setStyle(newface[style]);
	addText("Enjoy your new and improved face!");
	sendNext();
else
	addText("Hmm ... it looks like you don't have the coupon specifically for this place...sorry to say this, but without the coupon, there's no plastic surgery for you.");
	sendNext();
end