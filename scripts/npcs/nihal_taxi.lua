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
-- Camel Cab

if getMap() == 260020700 then
	tomap = 260000000;
elseif getMap() == 260020000 then
	tomap = 261000000;
end

addText("Will you move to #b#m260000000##k now? The price is #b1500 mesos#k.");
yes = askYesNo();

if yes == 0 then
	addText("Will you ride the #b#p2110005##k and move to #b#m" .. tomap .. "##k now? The price is #b1500 mesos#k.");
	sendNext();
else
	if giveMesos(-1500) then
		setMap(tomap);
	else
		addText("I am sorry, but I think you are short on mesos. I am afraid I can't let you ride this if you do not have enough money to do so. Please come back when you have enough money to use this.");
		sendNext();
	end
end