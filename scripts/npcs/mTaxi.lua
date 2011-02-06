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
-- VIP Cab (Lith Harbor, Ellinia)

addText("Hi there! This cab is for VIP customers only. Instead of just taking you to different towns like the regular cabs, we offer a much better service worthy of VIP class. It's a bit pricey, but... for only 10,000 mesos, we'll take you safely to the #bant tunnel#k.");
sendNext();

if getJob() == 0 then
	price = 1000;
	addText("We have a special 90% discount for beginners. The ant tunnel is located deep inside in the dungeon that's placed at the center of the Victoria Island, where #p1061001# is. Would you like to go there for #b1,000 mesos #k?");
else
	price = 10000;
	addText("The regular fee applies for all non-beginners. The ant tunnel is located deep inside in the dungeon that's placed at the center of the Victoria Island, where #p1061001# is. Would you like to go there for #b10,000 mesos #k?.");
end
yes = askYesNo();

if yes == 1 then
	if giveMesos(-price) then
		setMap(105070001);
	else
		addText("It looks like you don't have enough mesos. Sorry but you won't be able to use this without it.");
		sendNext();
	end
else
	addText("This town also has a lot to offer. Find us if and when you feel the need to go to the ant tunnel park.");
	sendNext();
end

