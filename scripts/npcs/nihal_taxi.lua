--[[
Copyright (C) 2008-2014 Vana Development Team

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

dofile("scripts/lua_functions/npcHelper.lua");

if getMap() == 260020700 then
	toMap = 260000000;
elseif getMap() == 260020000 then
	toMap = 261000000;
end

-- The following logic is intentional
-- It really was that messed up in GMS

addText("Will you move to " .. blue(mapRef(260000000)) .. " now? ");
addText("The price is " .. blue("1500 mesos") .. ".");
answer = askYesNo();

if answer == answer_no then
	addText("Will you ride the " .. blue(npcRef(2110005)) .. " and move to " .. blue(mapRef(toMap)) .. " now? ");
	addText("The price is " .. blue("1500 mesos") .. ".");
	sendNext();
else
	if giveMesos(-1500) then
		setMap(toMap);
	else
		addText("I am sorry, but I think you are short on mesos. ");
		addText("I am afraid I can't let you ride this if you do not have enough money to do so. ");
		addText("Please come back when you have enough money to use this.");
		sendNext();
	end
end