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
-- Adobis that runs the quests

addText("Well ... alright. You seem more than qualified for this. Which of these tasks do want to tackle on?\r\n");
addText("#b#L0# Explore the Dead Mine. (Level 1)#l\r\n");
addText("#b#L1# Observe the Zakum Dungeon. (Level 2)#l\r\n");
addText("#b#L2# Request for a refinery. (Level 3)#l\r\n");
addText("#b#L3# Get briefed for the quest.#l");
if getItemAmount(4001109) > 0 then
	addText("\r\n#b#L4# Enter the center of Lava.#l");
end
choice = askChoice();

if choice == 1 then

elseif choice == 4 then
	if not isInstance("fireDemon") then
		createInstance("fireDemon", 5 * 60, true);
		addInstancePlayer(getID());
		setMap(921100000);
	else
		addText("Someone is currently using the room. Try again later.");
		sendNext();
	end
end