--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Gachapon - The Nautilus : Mid Floor - Hallway

dofile("scripts/utils/gachaponHelper.lua");

-- General rules:
-- Global item chance is significantly reduced
-- All gear for town class; weight derived at 20 / itemLevel
-- 10 random gears between levels 10 and 40 from other town classes; weight derived at 10 / itemLevel
-- All skills for the town class
-- All weapon scrolls for the town class' weapons

commonEquips = {};

warriorEquips = {
	{1060017, ["weight"] = 0.33}, {1072047, ["weight"] = 0.50},
	{1302008, ["weight"] = 0.33}, {1072002, ["weight"] = 0.29},
	{1002055, ["weight"] = 0.50}, {1040028, ["weight"] = 0.31},
	{1041064, ["weight"] = 1.00}, {1072041, ["weight"] = 0.33},
	{1072127, ["weight"] = 0.25}, {1092000, ["weight"] = 0.67},
};

magicianEquips = {
	{1050010, ["weight"] = 0.77}, {1050008, ["weight"] = 0.77},
	{1072006, ["weight"] = 1.00}, {1082053, ["weight"] = 0.33},
	{1061022, ["weight"] = 0.77}, {1051026, ["weight"] = 0.26},
	{1051003, ["weight"] = 0.43}, {1382015, ["weight"] = 0.33},
	{1072074, ["weight"] = 0.40}, {1002013, ["weight"] = 0.33},
};

bowmanEquips = {
	{1082048, ["weight"] = 0.33}, {1061061, ["weight"] = 0.29},
	{1041056, ["weight"] = 0.33}, {1002161, ["weight"] = 0.33},
	{1002157, ["weight"] = 0.40}, {1002162, ["weight"] = 0.33},
	{1462048, ["weight"] = 0.25}, {1002158, ["weight"] = 0.40},
	{1041054, ["weight"] = 0.33}, {1002120, ["weight"] = 0.50},
};

thiefEquips = {
	{1040061, ["weight"] = 0.29}, {1060073, ["weight"] = 0.25},
	{1072031, ["weight"] = 0.67}, {1472013, ["weight"] = 0.29},
	{1051009, ["weight"] = 0.29}, {1472009, ["weight"] = 0.33},
	{1072036, ["weight"] = 0.33}, {1002184, ["weight"] = 0.25},
	{1002182, ["weight"] = 0.25}, {1061055, ["weight"] = 0.40},
};

pirateEquips = {
	{1002610, ["weight"] = 2.00}, {1002613, ["weight"] = 1.33},
	{1002616, ["weight"] = 1.00}, {1002619, ["weight"] = 0.80},
	{1002622, ["weight"] = 0.67}, {1002625, ["weight"] = 0.57},
	{1002628, ["weight"] = 0.50}, {1002631, ["weight"] = 0.40},
	{1002634, ["weight"] = 0.33}, {1002637, ["weight"] = 0.29},
	{1002640, ["weight"] = 0.25}, {1002643, ["weight"] = 0.22},
	{1002646, ["weight"] = 0.20}, {1002649, ["weight"] = 0.18},
	{1002780, ["weight"] = 0.17}, {1002794, ["weight"] = 0.17},
	{1052095, ["weight"] = 2.00}, {1052098, ["weight"] = 1.33},
	{1052101, ["weight"] = 1.00}, {1052104, ["weight"] = 0.80},
	{1052107, ["weight"] = 0.67}, {1052110, ["weight"] = 0.57},
	{1052113, ["weight"] = 0.50}, {1052116, ["weight"] = 0.40},
	{1052119, ["weight"] = 0.33}, {1052122, ["weight"] = 0.29},
	{1052125, ["weight"] = 0.25}, {1052128, ["weight"] = 0.22},
	{1052131, ["weight"] = 0.20}, {1052134, ["weight"] = 0.18},
	{1052159, ["weight"] = 0.17}, {1052164, ["weight"] = 0.17},
	{1072285, ["weight"] = 1.33}, {1072288, ["weight"] = 1.00},
	{1072291, ["weight"] = 0.80}, {1072294, ["weight"] = 0.67},
	{1072297, ["weight"] = 0.57}, {1072300, ["weight"] = 0.50},
	{1072303, ["weight"] = 0.40}, {1072306, ["weight"] = 0.33},
	{1072309, ["weight"] = 0.29}, {1072312, ["weight"] = 0.25},
	{1072315, ["weight"] = 0.22}, {1072318, ["weight"] = 0.20},
	{1072321, ["weight"] = 0.18}, {1072338, ["weight"] = 0.67},
	{1072359, ["weight"] = 0.17}, {1072365, ["weight"] = 0.17},
	{1082180, ["weight"] = 1.33}, {1082183, ["weight"] = 1.00},
	{1082186, ["weight"] = 0.80}, {1082189, ["weight"] = 0.67},
	{1082192, ["weight"] = 0.57}, {1082195, ["weight"] = 0.50},
	{1082198, ["weight"] = 0.40}, {1082201, ["weight"] = 0.33},
	{1082204, ["weight"] = 0.29}, {1082207, ["weight"] = 0.25},
	{1082210, ["weight"] = 0.22}, {1082213, ["weight"] = 0.20},
	{1082216, ["weight"] = 0.18}, {1082238, ["weight"] = 0.17},
	{1082243, ["weight"] = 0.17}, {1482000, ["weight"] = 2.00},
	{1482001, ["weight"] = 1.33}, {1482002, ["weight"] = 1.00},
	{1482003, ["weight"] = 0.80}, {1482004, ["weight"] = 0.67},
	{1482005, ["weight"] = 0.57}, {1482006, ["weight"] = 0.50},
	{1482007, ["weight"] = 0.40}, {1482008, ["weight"] = 0.33},
	{1482009, ["weight"] = 0.29}, {1482010, ["weight"] = 0.25},
	{1482011, ["weight"] = 0.22}, {1482012, ["weight"] = 0.20},
	{1482013, ["weight"] = 0.18}, {1482023, ["weight"] = 0.17},
	{1482024, ["weight"] = 0.17}, {1482034, ["weight"] = 0.17},
	{1492000, ["weight"] = 2.00}, {1492001, ["weight"] = 1.33},
	{1492002, ["weight"] = 1.00}, {1492003, ["weight"] = 0.80},
	{1492004, ["weight"] = 0.67}, {1492005, ["weight"] = 0.57},
	{1492006, ["weight"] = 0.50}, {1492007, ["weight"] = 0.40},
	{1492008, ["weight"] = 0.33}, {1492009, ["weight"] = 0.29},
	{1492010, ["weight"] = 0.25}, {1492011, ["weight"] = 0.22},
	{1492012, ["weight"] = 0.20}, {1492013, ["weight"] = 0.18},
	{1492023, ["weight"] = 0.17}, {1492025, ["weight"] = 0.17},
};

scrolls = {
	2044800, 2044801, 2044802, 2044803, 2044804, 2044805, 2044807, 2044809, 2044900, 2044901,
	2044902, 2044903, 2044904,
};

bullets = {
	-- TODO FIXME gachapon
};

skills = {
	-- 2290096 (which should be the start of these books given the other classes) is Maple Warrior 20, so obviously not a part of here
	{2290097, ["weight"] = mastery_book_20}, {2290098, ["weight"] = mastery_book_30},
	{2290099, ["weight"] = mastery_book_20}, {2290100, ["weight"] = mastery_book_30},
	{2290101, ["weight"] = mastery_book_20},
	{2290102, ["weight"] = mastery_book_20}, {2290103, ["weight"] = mastery_book_30},
	{2290104, ["weight"] = mastery_book_20}, {2290105, ["weight"] = mastery_book_30},
	{2290106, ["weight"] = mastery_book_20}, {2290107, ["weight"] = mastery_book_30},
	{2290108, ["weight"] = mastery_book_20}, --{2290109, ["weight"] = mastery_book_30}, -- Technically this book exists, but it's not used, it's Speed Infusion 30
	{2290110, ["weight"] = mastery_book_20}, {2290111, ["weight"] = mastery_book_30},
	{2290112, ["weight"] = mastery_book_20}, {2290113, ["weight"] = mastery_book_30},
	{2290114, ["weight"] = mastery_book_20},
	{2290115, ["weight"] = mastery_book_20}, {2290116, ["weight"] = mastery_book_30},
	{2290117, ["weight"] = mastery_book_20}, {2290118, ["weight"] = mastery_book_30},
	{2290119, ["weight"] = mastery_book_20}, {2290120, ["weight"] = mastery_book_30},
	{2290121, ["weight"] = mastery_book_20}, {2290122, ["weight"] = mastery_book_30},
	{2290123, ["weight"] = mastery_book_20},
	{2290124, ["weight"] = mastery_book_20},
};

items = merge(commonEquips, warriorEquips, magicianEquips, bowmanEquips, thiefEquips, pirateEquips, scrolls, bullets, skills);

gachapon({
	["items"] = items,
	-- Decrease the chance of getting things from the global item list
	["globalItemWeightModifier"] = .5,
});