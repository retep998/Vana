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
-- Hikari (warps to M/F Locker Room)

dofile("scripts/utils/npcHelper.lua");

addText("Would you like to enter the bathhouse? ");
addText("That'll be 300 mesos for you. ");
addText("And don't take the towels!");
answer = askYesNo();

if answer == answer_no then
	addText("Please come back some other time. ");
	sendOk();
else
	if giveMesos(-300) then
		if getGender() == gender_male then setMap(801000100);
		else setMap(801000200);
		end
	else
		addText("Please check your wallet or purse and see if you have 300 mesos to enter this place. ");
		addText("We have to keep the water hot you know...");
		sendNext();
	end
end