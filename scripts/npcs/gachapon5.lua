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
-- Gachapon - Dungeon : Sleepywood

items = {
	1102082, 1102043, 1102041, 1102042, 1102040, 1002587, 1002394, 1002393, 1082147, 1082149,
	1082148, 1082145, 1022047, 1050018, 1322021, 1442017, 1312012, 1432018, 2043104, 2043105,
	2043204, 2043205, 2043004, 2043005, 2043006, 2043007, 2044104, 2044105, 2044204, 2044205,
	2044004, 2044005, 2040604, 2040605, 2040610, 2040611, 2040606, 2040607, 2044504, 2044505,
	2041038, 2041039, 2041030, 2041031, 2041036, 2041037, 2041040, 2041041, 2041026, 2041027,
	2041032, 2041033, 2041034, 2041035, 2041028, 2041029, 2044704, 2044705, 2044604, 2044605,
	2043304, 2043305, 2040308, 2040309, 2040306, 2040307, 2040304, 2040305, 2040810, 2040811,
	2040808, 2040809, 2040812, 2040813, 2040814, 2040014, 2040015, 2040008, 2040009, 2040010,
	2040011, 2040012, 2040013, 2040510, 2040511, 2040508, 2040509, 2040518, 2040519, 2040520,
	2040520, 2044404, 2044405, 2040904, 2040905, 2040908, 2040909, 2040906, 2040907, 2040712,
	2040713, 2040714, 2040715, 2040716, 2040717, 2044304, 2044305, 2043804, 2043805, 2043704,
	2043705, 2040406, 2040407, 2040408, 2040409, 2040410, 2040411, 1402037, 1382037, 1382008,
	1372008, 1372032, 1372017, 1452018, 1472051, 1472054, 1332053, 1452044, 1462039, 1332049,
	1332050, 1312031, 1322052, 1302059, 1302026, 1302037, 1302049, 1302063, 1302021, 1442002,
	1442045, 1432038, 1432017, 1432016, 1432015, 1412026, 1422028, 1422011, 1402014, 1402013,
	1402017, 1402036, 1051017
};

if getItemAmount(5220000) >= 1 then
	addText("You have a #bGachapon Ticket#k. Would you like to use it?");
	yes = askYesNo();
	
	if yes == 1 then
		giveItem(5220000, -1);
		random = getRandomNumber(#items); -- generate random number between 1 and the amount of items in the items array
		giveItem(items[random], 1);
		addText("You have obtained #b#t" .. items[random] .. "##k.");
		sendNext();
	end
else
	addText("Here's Gachapon.");
	sendOK();
end
