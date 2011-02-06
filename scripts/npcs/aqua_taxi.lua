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
-- Dolphin (Aquarium)

prices = {1000, 10000};
maps = {230030200, 251000100};

addText("Oceans are all connected to each other. Places you can't reach by foot can be easily reached oversea. How about taking #bDolphin Taxi#k with us today? \r\n");
addText("#L0# Go to #bThe Sharp Unknown#k after paying 1000mesos.#l\r\n");
addText("#L1# Go to #bHerb Town#k after paying 10000mesos.#l");
where = askChoice() + 1;

if giveMesos(-prices[where]) then -- Go to Sharp Unknown/Herb Town
	setMap(maps[where]);
else
	addText("I don't think you have enough money...");
	sendNext();
end
