--[[
Copyright (C) 2008-2009 Vana Development Team and Biscuit

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
-- Bell, warps to/from Kerning and NLC

map = getMap();
cost = 5000;
item = 0;

if map == 103000100 then
	addText("Hi, I can take you to New Leaf City (NLC) for only #b" .. cost .. " mesos#k. Would you like to go?");
	yes = askYesNo();

	if yes == 1 then
		-- TODO: Figure out the conditions under which a different ticket is given out and how much it costs
		item = 4031711;
	else
		addText("Ok, come talk to me again when you want to go to NLC.");
		sendOK();
	end
elseif map == 600010001 then
	addText("Hi, I can take you back to Kerning City for only #b" .. cost .. " mesos#k. Would you like to go?");
	yes = askYesNo();

	if yes == 1 then
		-- TODO: Figure out the conditions under which a different ticket is given out and how much it costs
		item = 4031713;
	else
		addText("Ok, come talk to me again when you want to go back to Kerning City.");
		sendOK();
	end
elseif map == 600010004 then
	-- Need Bell's exit text
elseif map == 600010003 then
	-- Same here
end

if item ~= 0 then
	if giveMesos(-cost) then
		giveItem(item, 1);
	else
		addText("I'm sorry, but you don't have enough money. It costs #b" .. cost .. " Mesos#k. ");
		sendOK();
	end
end