--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Camel Cab

if getMap() == 260020700 then
	tomap = 260000000;
elseif getMap() == 260020000 then
	tomap = 261000000;
end

if state == 0 then
	addText("Will you ride the #b#p2110005##k and move to #b#m" .. tomap .. "##k now? The price is #b1500 mesos#k.");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 0 then
		addText("Hmmm ... too busy to do it right now? If you feel like doing it, though, come back and find me.");
		sendNext();
	else
		if getMesos() < 1500 then
			addText("I am sorry, but I think you are short on mesos. I am afraid I can't let you ride this if you do not have enough money to do so. Please come back when you have enough money to use this.");
			sendNext();
		else
			giveMesos(-1500);
			setMap(tomap);
		end
	end
	endNPC();
end