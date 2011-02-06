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
-- Hotel Receptionist, Sleepywood sauna entry NPC

addText("Welcome. We're the Sleepywood Hotel. Our hotel works hard to serve you the best at all times. If you are tired and worn out from hunting, how about a relaxing stay at our hotel?");
sendNext();

addText("We offer two kinds of rooms for service. Please choose the one of your liking.\r\n#b#L0#Regular sauna(499 mesos per use)#l\r\n#L1#VIP sauna(999 mesos per use)#l");
what = askChoice();

if what == 0 then
	price = 499;
	map = 105040401;
	addText("You've chosen the regular sauna. Your HP and MP will recover fast and you can even purchase some items there. Are you sure you want to go in?");
elseif what == 1 then
	price = 999;
	map = 105040402;
	addText("You've chosen the VIP sauna. Your HP and MP will recover even faster than that of the regular sauna and you can even find a special item in there. Are you sure you want to go in?");
end
yes = askYesNo();

if yes == 1 then
	if giveMesos(-price) then
		setMap(map);
	else
		addText("I'm sorry. It looks like you don't have enough mesos. It will cost you " .. price .. " mesos to stay at our hotel.");
		sendOK();
	end
else
	addText("We offer other kinds of services, too, so please think carefully and then make your decision.");
	sendOK();
end