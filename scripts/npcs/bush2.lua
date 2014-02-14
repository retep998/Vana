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
-- A Pile of Herbs

-- TODO FIXME implement distance properly
-- You can't see the inside of the bush very well because you're too far. Go a little closer.

if isQuestActive(2051) then
	item = 4031032;
	addText("Are you sure you want to take #b#t" .. item .. "##k with you?");
	answer = askYesNo();
	
	if answer == 1 then
		if giveItem(item, 1) then
			setMap(101000000);
		else
			addText("Your etc. inventory seems to be full. ");
			addText("Please make room in order to take the item.");
			sendNext();
		end
	end
else
	chance = getRandomNumber(30);
	item = nil;
	qty = nil;
	if chance == 30 then
		item = 1032013;
		qty = 1;
	else
		items = {4020007, 4020008, 4010006};
		item = items[getRandomNumber(#items)];
		qty = 2;
	end

	if giveItem(item, qty) then
		setMap(101000000);
	else
		addText("You need to make some room for your equipment and etc. inventory in order to take in the items you've discovered at the herbal bush. ");
		addText("Please check again after making the adjustment.");
		sendNext();
	end	
end