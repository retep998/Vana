--[[
Copyright (C) 2008 Vana Development Team

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
-- Jake - Subway Worker (Kerning City)

site = {"construction site B1", "construction site B2", "construction site B3"};
cost = {500, 1200, 2000};

if getLevel() < 20 then
	addText("You must purchase the ticket to enter. Once you have made the purchase, you can enter through The Ticket Gate on the right. What would you like to buy?");
	sendNext();
else
	addText("You must purchase the ticket to enter. Once you have made the purchase, you can enter through The Ticket Gate on the right. What would you like to buy?");
	if getLevel() >= 20 then
		addText("\r\n#L1##bConstruction Site B1#k#l");
	end
	if getLevel() >= 30 then
		addText("\r\n#L2##bConstruction Site B2#k#l");
	end
	if getLevel() >= 40 then
		addText("\r\n#L3##bConstruction Site B3#k#l");
	end
	choice = askChoice();

	addText("Will you purchase the ticket to #b" .. site[choice] .. "#k? It'll cost you " .. cost[choice] .. " mesos. Before making the purchase, please make sure you have an empty slot on your etc. inventory.");
	yes = askYesNo();

	if yes == 0 then
		addText("You can enter the premise once you have bought the ticket. ");
		addText("I heard there are strange devices in there everywhere but in the end, rare precious items await you. ");
		addText("So let me know if you ever decide to change your mind.");
		sendOK();
	else
		item = 0;
		text = "You can insert the ticket in the The Ticket Gate. I heard ";

		if choice == 1 then
			item = 4031036;
			text = text .. "Area 1";
		elseif choice == 2 then
			item = 4031037;
			text = text .. "Area 2";
		elseif choice == 3 then
			item = 4031038;
			text = text .. "Area 3";
		end

		if (item == 0) or (getMesos() < cost[choice]) or not giveItem(item, 1) then
			addText("Are you lacking mesos? Check and see if you have an empty slot on your etc. inventory or not.");
		else
			giveMesos(-cost[choice]);
			text = text .. " has some precious items available but with so many traps all over the place most come back out early. Please be safe.";
			addText(text);
		end
		sendOK();
	end
end