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
-- Gachapon machines

function gachapon(items)
	if getItemAmount(5220000) >= 1 then
		addText("You have a #b#t5220000##k. Would you like to use it?");
		yes = askYesNo();

		if yes == 1 then
			for i = 1, 4 do
				if getOpenSlots(i) == 0 then
					addText("Please make room on your item inventory and then try again.");
					sendNext();
					return;
				end
			end

			item = items[getRandomNumber(#items)];
			if type(item) == "number" then
				item = {item, ["qty"] = 1};
			else
				if type(item["qty"]) == "table" then
					maximum = item["qty"]["maximum"];
					minimum = item["qty"]["minimum"];
					item["qty"] = (getRandomNumber(maximum - minimum + 1) - 1) + minimum;
				end
			end
			
			giveItem(5220000, -1);
			giveItem(item[1], item["qty"]);

			addText("You have obtained #b#t" .. item[1] .. "##k.");
			sendNext();
		else
			addText("Please come again!");
			sendNext();
		end
	else
		addText("Here's Gachapon.");
		sendOk();
	end
end