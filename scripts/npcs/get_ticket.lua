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
-- Numerous NPCs

if getMap() == 240000111 then
	-- Waiting room for Leafre -> Orbis
	addText("Do you want to leave the waiting room? You can, but the ticket is NOT refundable. Are you sure you still want to leave this room?");
	yes = askYesNo();

	if yes == 1 then
		setMap(240000100);
	else
		addText("You'll get to your destination in a few. Go ahead and talk to other people, and before you know it, you'll be there already.");
		sendNext();
	end
end