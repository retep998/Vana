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
-- Assistant Bonnie

addText("I can guide you to the Wedding. Which one suits you?\r\n");
addText("#b#L0#I am now ready to get Married in Chapel.#l\r\n");
addText("#L1#I am invited to the wedding!#l#k");
value = askChoice();

if value == 0 then
	addText("You need to be in #ba 2-person party with your fiancee or fiance#kto get married.");
	sendNext();
elseif value == 1 then
	addText("Welcome! I can tell that you're a guest of the Bride and Groom, would you like to enter the Chapel?");
	yes = askYesNo();

	if yes == 1 then
		addText("I apologize, but the Wedding hasn't started yet. When it does, I'll be sure to let you in. ");
		sendNext();
	else
		addText("Step aside then, there are others ready to go in.");
		sendNext();
	end
end
