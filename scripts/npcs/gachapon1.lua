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
-- Gachapon - Victoria Road : Henesys Market

dofile("scripts/utils/gachaponHelper.lua");

-- TODO FIXME gachapon
-- The TCG equips need to be culled out from all Gachapons
-- Potentially the Crimsonwood Forest equips too (Dawn Raven etc.)

-- General rules:
-- Global item chance is significantly reduced
-- All gear for town class; weight derived at 20 / itemLevel
-- 10 random gears between levels 10 and 40 from other town classes; weight derived at 10 / itemLevel
-- All skills for the town class
-- All weapon scrolls for the town class' weapons
-- Includes some common equips because Henesys is the de-facto common town

commonEquips = {
	1432009, 1302022, 1322027, 1062000, 1002033, 1092022, 1302021, 1322009, 1002012, 1322012,
};

warriorEquips = {
	{1312007, ["weight"] = 0.25}, {1002093, ["weight"] = 0.25},
	{1051016, ["weight"] = 0.29}, {1040030, ["weight"] = 0.29},
	{1060009, ["weight"] = 0.67}, {1060000, ["weight"] = 0.25},
	{1082025, ["weight"] = 0.25}, {1402002, ["weight"] = 0.33},
	{1092000, ["weight"] = 0.67}, {1072041, ["weight"] = 0.33},
};

magicianEquips = {
	{1051023, ["weight"] = 0.26}, {1002035, ["weight"] = 0.33},
	{1061028, ["weight"] = 0.56}, {1040019, ["weight"] = 0.56},
	{1002152, ["weight"] = 0.25}, {1002155, ["weight"] = 0.25},
	{1041018, ["weight"] = 0.77}, {1050031, ["weight"] = 0.30},
	{1002102, ["weight"] = 0.67}, {1082028, ["weight"] = 0.40},
};

bowmanEquips = {
	{1002010, ["weight"] = 2.00}, {1002057, ["weight"] = 2.00},
	{1002112, ["weight"] = 1.33}, {1002113, ["weight"] = 1.33},
	{1002114, ["weight"] = 1.33}, {1002115, ["weight"] = 1.33},
	{1002116, ["weight"] = 1.33}, {1002117, ["weight"] = 1.00},
	{1002118, ["weight"] = 1.00}, {1002119, ["weight"] = 1.00},
	{1002120, ["weight"] = 1.00}, {1002121, ["weight"] = 1.00},
	{1002135, ["weight"] = 0.57}, {1002136, ["weight"] = 0.57},
	{1002137, ["weight"] = 0.57}, {1002138, ["weight"] = 0.57},
	{1002139, ["weight"] = 0.57}, {1002156, ["weight"] = 0.80},
	{1002157, ["weight"] = 0.80}, {1002158, ["weight"] = 0.80},
	{1002159, ["weight"] = 0.80}, {1002160, ["weight"] = 0.80},
	{1002161, ["weight"] = 0.67}, {1002162, ["weight"] = 0.67},
	{1002163, ["weight"] = 0.67}, {1002164, ["weight"] = 0.67},
	{1002165, ["weight"] = 0.67}, {1002166, ["weight"] = 0.50},
	{1002167, ["weight"] = 0.50}, {1002168, ["weight"] = 0.50},
	{1002169, ["weight"] = 0.50}, {1002170, ["weight"] = 0.50},
	{1002211, ["weight"] = 0.40}, {1002212, ["weight"] = 0.40},
	{1002213, ["weight"] = 0.40}, {1002214, ["weight"] = 0.40},
	{1002267, ["weight"] = 0.33}, {1002268, ["weight"] = 0.33},
	{1002269, ["weight"] = 0.33}, {1002270, ["weight"] = 0.33},
	{1002275, ["weight"] = 0.25}, {1002276, ["weight"] = 0.25},
	{1002277, ["weight"] = 0.25}, {1002278, ["weight"] = 0.25},
	{1002286, ["weight"] = 0.29}, {1002287, ["weight"] = 0.29},
	{1002288, ["weight"] = 0.29}, {1002289, ["weight"] = 0.29},
	{1002402, ["weight"] = 0.22}, {1002403, ["weight"] = 0.22},
	{1002404, ["weight"] = 0.22}, {1002405, ["weight"] = 0.22},
	{1002406, ["weight"] = 0.20}, {1002407, ["weight"] = 0.20},
	{1002408, ["weight"] = 0.20}, {1002547, ["weight"] = 0.18},
	{1002580, ["weight"] = 0.57}, {1002749, ["weight"] = 0.40},
	{1002778, ["weight"] = 0.17}, {1002792, ["weight"] = 0.17},
	{1002940, ["weight"] = 0.17}, {1040003, ["weight"] = 1.00},
	{1040007, ["weight"] = 1.33}, {1040008, ["weight"] = 2.00},
	{1040011, ["weight"] = 1.33}, {1040022, ["weight"] = 0.80},
	{1040023, ["weight"] = 0.80}, {1040024, ["weight"] = 0.80},
	{1040025, ["weight"] = 0.80}, {1040067, ["weight"] = 0.67},
	{1040068, ["weight"] = 0.67}, {1040069, ["weight"] = 0.67},
	{1040070, ["weight"] = 0.67}, {1040071, ["weight"] = 2.00},
	{1040072, ["weight"] = 0.57}, {1040073, ["weight"] = 0.57},
	{1040074, ["weight"] = 0.57}, {1040075, ["weight"] = 0.57},
	{1040076, ["weight"] = 0.57}, {1040079, ["weight"] = 0.50},
	{1040080, ["weight"] = 0.50}, {1040081, ["weight"] = 0.50},
	{1041007, ["weight"] = 2.00}, {1041008, ["weight"] = 1.33},
	{1041013, ["weight"] = 1.00}, {1041027, ["weight"] = 1.00},
	{1041028, ["weight"] = 1.00}, {1041032, ["weight"] = 0.80},
	{1041033, ["weight"] = 0.80}, {1041034, ["weight"] = 0.80},
	{1041035, ["weight"] = 0.80}, {1041054, ["weight"] = 0.67},
	{1041055, ["weight"] = 0.67}, {1041056, ["weight"] = 0.67},
	{1041061, ["weight"] = 2.00}, {1041062, ["weight"] = 1.33},
	{1041063, ["weight"] = 1.33}, {1041065, ["weight"] = 0.57},
	{1041066, ["weight"] = 0.57}, {1041067, ["weight"] = 0.57},
	{1041068, ["weight"] = 0.57}, {1041069, ["weight"] = 0.57},
	{1041081, ["weight"] = 0.50}, {1041082, ["weight"] = 0.50},
	{1041083, ["weight"] = 0.50}, {1050051, ["weight"] = 0.40},
	{1050052, ["weight"] = 0.40}, {1050058, ["weight"] = 0.33},
	{1050059, ["weight"] = 0.33}, {1050060, ["weight"] = 0.33},
	{1050061, ["weight"] = 0.29}, {1050062, ["weight"] = 0.29},
	{1050063, ["weight"] = 0.29}, {1050064, ["weight"] = 0.29},
	{1050075, ["weight"] = 0.25}, {1050076, ["weight"] = 0.25},
	{1050077, ["weight"] = 0.25}, {1050078, ["weight"] = 0.25},
	{1050088, ["weight"] = 0.22}, {1050089, ["weight"] = 0.22},
	{1050090, ["weight"] = 0.22}, {1050091, ["weight"] = 0.22},
	{1050106, ["weight"] = 0.20}, {1050107, ["weight"] = 0.20},
	{1050108, ["weight"] = 0.20}, {1051037, ["weight"] = 0.40},
	{1051038, ["weight"] = 0.40}, {1051039, ["weight"] = 0.40},
	{1051041, ["weight"] = 0.33}, {1051042, ["weight"] = 0.33},
	{1051043, ["weight"] = 0.33}, {1051062, ["weight"] = 0.29},
	{1051063, ["weight"] = 0.29}, {1051064, ["weight"] = 0.29},
	{1051065, ["weight"] = 0.29}, {1051066, ["weight"] = 0.25},
	{1051067, ["weight"] = 0.25}, {1051068, ["weight"] = 0.25},
	{1051069, ["weight"] = 0.25}, {1051082, ["weight"] = 0.22},
	{1051083, ["weight"] = 0.22}, {1051084, ["weight"] = 0.22},
	{1051085, ["weight"] = 0.22}, {1051105, ["weight"] = 0.20},
	{1051106, ["weight"] = 0.20}, {1051107, ["weight"] = 0.20},
	{1052071, ["weight"] = 0.18}, {1052157, ["weight"] = 0.17},
	{1052162, ["weight"] = 0.17}, {1060056, ["weight"] = 0.67},
	{1060057, ["weight"] = 0.67}, {1060058, ["weight"] = 0.67},
	{1060059, ["weight"] = 0.67}, {1060061, ["weight"] = 0.57},
	{1060062, ["weight"] = 0.57}, {1060063, ["weight"] = 0.57},
	{1060064, ["weight"] = 0.57}, {1060065, ["weight"] = 0.57},
	{1060068, ["weight"] = 0.50}, {1060069, ["weight"] = 0.50},
	{1060070, ["weight"] = 0.50}, {1061006, ["weight"] = 1.33},
	{1061009, ["weight"] = 2.00}, {1061024, ["weight"] = 1.00},
	{1061025, ["weight"] = 1.00}, {1061026, ["weight"] = 1.00},
	{1061050, ["weight"] = 0.67}, {1061051, ["weight"] = 0.67},
	{1061052, ["weight"] = 0.67}, {1061057, ["weight"] = 2.00},
	{1061058, ["weight"] = 1.33}, {1061059, ["weight"] = 1.33},
	{1061060, ["weight"] = 0.57}, {1061061, ["weight"] = 0.57},
	{1061062, ["weight"] = 0.57}, {1061063, ["weight"] = 0.57},
	{1061064, ["weight"] = 0.57}, {1061080, ["weight"] = 0.50},
	{1061081, ["weight"] = 0.50}, {1061082, ["weight"] = 0.50},
	{1062002, ["weight"] = 1.00}, {1062004, ["weight"] = 2.00},
	{1062006, ["weight"] = 0.80}, {1072015, ["weight"] = 2.00},
	{1072016, ["weight"] = 1.33}, {1072025, ["weight"] = 1.00},
	{1072026, ["weight"] = 1.00}, {1072027, ["weight"] = 0.80},
	{1072034, ["weight"] = 0.80}, {1072059, ["weight"] = 2.00},
	{1072060, ["weight"] = 1.33}, {1072061, ["weight"] = 1.33},
	{1072067, ["weight"] = 1.00}, {1072068, ["weight"] = 1.00},
	{1072069, ["weight"] = 0.80}, {1072079, ["weight"] = 0.67},
	{1072080, ["weight"] = 0.67}, {1072081, ["weight"] = 0.67},
	{1072082, ["weight"] = 0.67}, {1072083, ["weight"] = 0.67},
	{1072101, ["weight"] = 0.57}, {1072102, ["weight"] = 0.57},
	{1072103, ["weight"] = 0.57}, {1072118, ["weight"] = 0.50},
	{1072119, ["weight"] = 0.50}, {1072120, ["weight"] = 0.50},
	{1072121, ["weight"] = 0.50}, {1072122, ["weight"] = 0.40},
	{1072123, ["weight"] = 0.40}, {1072124, ["weight"] = 0.40},
	{1072125, ["weight"] = 0.40}, {1072144, ["weight"] = 0.33},
	{1072145, ["weight"] = 0.33}, {1072146, ["weight"] = 0.33},
	{1072164, ["weight"] = 0.29}, {1072165, ["weight"] = 0.29},
	{1072166, ["weight"] = 0.29}, {1072167, ["weight"] = 0.29},
	{1072170, ["weight"] = 0.67}, {1072182, ["weight"] = 0.25},
	{1072183, ["weight"] = 0.25}, {1072184, ["weight"] = 0.25},
	{1072185, ["weight"] = 0.25}, {1072203, ["weight"] = 0.22},
	{1072204, ["weight"] = 0.22}, {1072205, ["weight"] = 0.22},
	{1072227, ["weight"] = 0.20}, {1072228, ["weight"] = 0.20},
	{1072229, ["weight"] = 0.20}, {1072269, ["weight"] = 0.18},
	{1072345, ["weight"] = 0.40}, {1072357, ["weight"] = 0.17},
	{1072363, ["weight"] = 0.17}, {1082012, ["weight"] = 1.33},
	{1082013, ["weight"] = 1.00}, {1082014, ["weight"] = 1.00},
	{1082015, ["weight"] = 1.00}, {1082016, ["weight"] = 0.80},
	{1082017, ["weight"] = 0.80}, {1082018, ["weight"] = 0.80},
	{1082048, ["weight"] = 0.67}, {1082049, ["weight"] = 0.67},
	{1082050, ["weight"] = 0.67}, {1082068, ["weight"] = 0.57},
	{1082069, ["weight"] = 0.57}, {1082070, ["weight"] = 0.57},
	{1082071, ["weight"] = 0.50}, {1082072, ["weight"] = 0.50},
	{1082073, ["weight"] = 0.50}, {1082083, ["weight"] = 0.40},
	{1082084, ["weight"] = 0.40}, {1082085, ["weight"] = 0.40},
	{1082089, ["weight"] = 0.33}, {1082090, ["weight"] = 0.33},
	{1082091, ["weight"] = 0.33}, {1082106, ["weight"] = 0.29},
	{1082107, ["weight"] = 0.29}, {1082108, ["weight"] = 0.29},
	{1082109, ["weight"] = 0.25}, {1082110, ["weight"] = 0.25},
	{1082111, ["weight"] = 0.25}, {1082112, ["weight"] = 0.25},
	{1082125, ["weight"] = 0.22}, {1082126, ["weight"] = 0.22},
	{1082127, ["weight"] = 0.22}, {1082158, ["weight"] = 0.20},
	{1082159, ["weight"] = 0.20}, {1082160, ["weight"] = 0.20},
	{1082163, ["weight"] = 0.18}, {1082236, ["weight"] = 0.17},
	{1082241, ["weight"] = 0.17}, {1452000, ["weight"] = 0.80},
	{1452001, ["weight"] = 1.00}, {1452002, ["weight"] = 2.00},
	{1452003, ["weight"] = 1.33}, {1452004, ["weight"] = 0.33},
	{1452005, ["weight"] = 0.67}, {1452006, ["weight"] = 0.57},
	{1452007, ["weight"] = 0.50}, {1452008, ["weight"] = 0.40},
	{1452009, ["weight"] = 0.29}, {1452010, ["weight"] = 0.29},
	{1452011, ["weight"] = 0.29}, {1452012, ["weight"] = 0.25},
	{1452013, ["weight"] = 0.25}, {1452014, ["weight"] = 0.25},
	{1452015, ["weight"] = 0.25}, {1452017, ["weight"] = 0.22},
	{1452018, ["weight"] = 0.31}, {1452019, ["weight"] = 0.20},
	{1452020, ["weight"] = 0.20}, {1452021, ["weight"] = 0.20},
	{1452023, ["weight"] = 0.37}, {1452025, ["weight"] = 0.22},
	{1452026, ["weight"] = 0.22}, {1452044, ["weight"] = 0.18},
	{1452052, ["weight"] = 0.29}, {1452057, ["weight"] = 0.17},
	{1452059, ["weight"] = 0.17}, {1452060, ["weight"] = 0.20},
	{1462000, ["weight"] = 0.71}, {1462001, ["weight"] = 1.67},
	{1462002, ["weight"] = 1.11}, {1462003, ["weight"] = 0.91},
	{1462004, ["weight"] = 0.63}, {1462005, ["weight"] = 0.53},
	{1462006, ["weight"] = 0.48}, {1462007, ["weight"] = 0.40},
	{1462008, ["weight"] = 0.33}, {1462009, ["weight"] = 0.29},
	{1462010, ["weight"] = 0.25}, {1462011, ["weight"] = 0.25},
	{1462012, ["weight"] = 0.25}, {1462013, ["weight"] = 0.25},
	{1462015, ["weight"] = 0.20}, {1462016, ["weight"] = 0.20},
	{1462017, ["weight"] = 0.20}, {1462018, ["weight"] = 0.22},
	{1462021, ["weight"] = 0.22}, {1462022, ["weight"] = 0.22},
	{1462039, ["weight"] = 0.18}, {1462046, ["weight"] = 0.29},
	{1462047, ["weight"] = 2.00}, {1462048, ["weight"] = 0.50},
	{1462049, ["weight"] = 0.25}, {1462050, ["weight"] = 0.17},
	{1462051, ["weight"] = 0.17}, {1462052, ["weight"] = 0.25},
	{1462053, ["weight"] = 0.22}, {1462054, ["weight"] = 0.22},
	{1462055, ["weight"] = 0.25},
};

thiefEquips = {
	{1002128, ["weight"] = 0.50}, {1472005, ["weight"] = 0.50},
	{1060032, ["weight"] = 0.50}, {1472017, ["weight"] = 0.25},
	{1061040, ["weight"] = 0.45}, {1040035, ["weight"] = 0.67},
	{1061043, ["weight"] = 0.33}, {1040062, ["weight"] = 0.29},
	{1051007, ["weight"] = 0.29}, {1061046, ["weight"] = 0.33},
};

pirateEquips = {
	{1052104, ["weight"] = 0.40}, {1052110, ["weight"] = 0.29},
	{1082183, ["weight"] = 0.50}, {1482003, ["weight"] = 0.40},
	{1492001, ["weight"] = 0.67}, {1072285, ["weight"] = 0.67},
	{1002613, ["weight"] = 0.67}, {1482006, ["weight"] = 0.25},
	{1082192, ["weight"] = 0.29}, {1492005, ["weight"] = 0.29},
};

arrows = {
	{2061004, ["qty"] = getMaxStackSize(2061004)}, {2060004, ["qty"] = getMaxStackSize(2060004)},
	{2061003, ["qty"] = getMaxStackSize(2061003)}, {2060003, ["qty"] = getMaxStackSize(2060003)},
	{2061002, ["qty"] = getMaxStackSize(2061002)}, {2060002, ["qty"] = getMaxStackSize(2060002)},
	{2061001, ["qty"] = getMaxStackSize(2061001)}, {2060001, ["qty"] = getMaxStackSize(2060001)},
};

scrolls = {
	2044605, 2044604, 2044602, 2044601, 2044600, 2044505, 2044504, 2044502, 2044501, 2044500,
};

skills = {
	{2290052, ["weight"] = mastery_book_20}, {2290053, ["weight"] = mastery_book_30},
	{2290054, ["weight"] = mastery_book_20}, {2290055, ["weight"] = mastery_book_30},
	{2290056, ["weight"] = mastery_book_20}, {2290057, ["weight"] = mastery_book_30},
	{2290058, ["weight"] = mastery_book_20}, {2290059, ["weight"] = mastery_book_30},
	{2290060, ["weight"] = mastery_book_20}, {2290061, ["weight"] = mastery_book_30},
	{2290062, ["weight"] = mastery_book_20}, {2290063, ["weight"] = mastery_book_30},
	{2290064, ["weight"] = mastery_book_20}, {2290065, ["weight"] = mastery_book_30},
	{2290066, ["weight"] = mastery_book_20}, {2290067, ["weight"] = mastery_book_30},
	{2290068, ["weight"] = mastery_book_20}, {2290069, ["weight"] = mastery_book_30},
	{2290070, ["weight"] = mastery_book_20}, {2290071, ["weight"] = mastery_book_30},
	{2290072, ["weight"] = mastery_book_20}, {2290073, ["weight"] = mastery_book_30},
	{2290074, ["weight"] = mastery_book_20}, {2290075, ["weight"] = mastery_book_30},
};

items = merge(commonEquips, warriorEquips, magicianEquips, bowmanEquips, thiefEquips, pirateEquips, arrows, scrolls, skills);

gachapon({
	["items"] = items,
	-- Decrease the chance of getting things from the global item list
	["globalItemWeightModifier"] = .5,
});