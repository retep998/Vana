--[[
Copyright (C) 2008-2011 Vana Development Team and Biscuit

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

function isNoob()
	return getLevel() <= 10;
end

if isNoob() then
	cost = 1000;
end

if map == 103000100 then
	addText("Hi, I can take you to New Leaf City (NLC) for only #b" .. cost .. " mesos#k. Would you like to go?");
	yes = askYesNo();

	if yes == 1 then
		if isNoob() then
			item = 4031711;
		else
			item = 4031712;
		end
	else
		addText("Ok, come talk to me again when you want to go to NLC.");
		sendOK();
	end
elseif map == 600010001 then
	addText("Hi, I can take you back to Kerning City for only #b" .. cost .. " mesos#k. Would you like to go?");
	yes = askYesNo();

	if yes == 1 then
		if isNoob() then
			item = 4031713;
		else
			item = 4031714;
		end
	else
		addText("Ok, come talk to me again when you want to go back to Kerning City.");
		sendOK();
	end
elseif map == 600010004 then
	addText("Do you want to go back to Kerning City subway station now?");
	yes = askYesNo();

	if yes == 0 then
		addText("Okay, Please wait~!");
		sendNext();
	else
		setMap(103000100);
	end
elseif map == 600010002 then
	addText("Do you want to go back to New Leaf City subway station now?");
	yes = askYesNo();

	if yes == 0 then
		addText("Okay, Please wait~!");
		sendNext();
	else
		setMap(600010001);
	end
end

if item ~= 0 then
	if giveMesos(-cost) then
		giveItem(item, 1);
	else
		addText("I'm sorry, but you don't have enough money. It costs #b" .. cost .. " Mesos#k. ");
		sendOK();
	end
end