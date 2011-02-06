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
-- Joel and Syras, tickets to Orbis

if getLevel() < 10 then
	addText("Your level seems to be too low for this. We do not allow anyone below Level 10 to get on this ride, for the sake of safety.");
	sendNext();
else
	if getLevel() < 30 then
		item = 4031044; -- Ticket to Orbis (Basic)
		price = 1000;
	else
		item = 4031045; -- Ticket to Orbis (Regular)
		price = 5000;
	end
	
	addText("Hello, I'm in charge of selling tickets for the ship ride to Orbis Station of Ossyria. The ride to Orbis takes off every 15 minutes, beginning on the hour, and it'll cost you #b" .. price .. " mesos#k. Are you sure you want to purchase #b#t" .. item .. "##k?");
	yes = askYesNo();

	if yes == 1 then
		if getMesos() < price or not hasOpenSlotsFor(item) then
			addText("Are you sure you have #b" .. price .. " mesos#k? If so, then I urge you to check your etc. inventory, and see if it's full or not.");
			sendOK();
		else
			giveMesos(-price);
			giveItem(item, 1);
		end
	else
		addText("You must have some business to take care of here, right?");
		sendNext();
	end
end