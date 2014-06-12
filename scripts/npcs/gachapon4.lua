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
-- Gachapon - Victoria Road : Kerning City

dofile("scripts/lua_functions/gachaponHelper.lua");

-- General rules:
-- Global item chance is significantly reduced
-- All gear for town class; weight derived at 20 / itemLevel
-- 10 random gears between levels 10 and 40 from other town classes; weight derived at 10 / itemLevel
-- All skills for the town class
-- All weapon scrolls for the town class' weapons


commonEquips = {};

warriorEquips = {
	{1002023, ["weight"] = 0.33}, {1082001, ["weight"] = 0.40},
	{1041084, ["weight"] = 0.25}, {1060017, ["weight"] = 0.33},
	{1422008, ["weight"] = 0.29}, {1402018, ["weight"] = 0.67},
	{1060019, ["weight"] = 0.33}, {1072047, ["weight"] = 0.50},
	{1040009, ["weight"] = 0.67}, {1060060, ["weight"] = 0.40},
};

magicianEquips = {
	{1050029, ["weight"] = 0.30}, {1072045, ["weight"] = 1.00},
	{1050028, ["weight"] = 0.36}, {1372001, ["weight"] = 0.30},
	{1051024, ["weight"] = 0.26}, {1061049, ["weight"] = 0.30},
	{1072091, ["weight"] = 0.29}, {1041030, ["weight"] = 0.56},
	{1041042, ["weight"] = 0.36}, {1002036, ["weight"] = 0.33},
};

bowmanEquips = {
	{1072103, ["weight"] = 0.29}, {1072034, ["weight"] = 0.40},
	{1072069, ["weight"] = 0.40}, {1062004, ["weight"] = 1.00},
	{1082050, ["weight"] = 0.33}, {1040003, ["weight"] = 0.50},
	{1061064, ["weight"] = 0.29}, {1040074, ["weight"] = 0.29},
	{1061051, ["weight"] = 0.33}, {1040079, ["weight"] = 0.25},
};

thiefEquips = {
	{1002107, ["weight"] = 1.33}, {1002108, ["weight"] = 1.33},
	{1002109, ["weight"] = 1.33}, {1002110, ["weight"] = 1.33},
	{1002111, ["weight"] = 1.33}, {1002122, ["weight"] = 2.00},
	{1002123, ["weight"] = 2.00}, {1002124, ["weight"] = 2.00},
	{1002125, ["weight"] = 2.00}, {1002126, ["weight"] = 2.00},
	{1002127, ["weight"] = 1.00}, {1002128, ["weight"] = 1.00},
	{1002129, ["weight"] = 1.00}, {1002130, ["weight"] = 1.00},
	{1002131, ["weight"] = 1.00}, {1002146, ["weight"] = 0.80},
	{1002147, ["weight"] = 0.80}, {1002148, ["weight"] = 0.80},
	{1002149, ["weight"] = 0.80}, {1002150, ["weight"] = 0.80},
	{1002171, ["weight"] = 0.67}, {1002172, ["weight"] = 0.67},
	{1002173, ["weight"] = 0.67}, {1002174, ["weight"] = 0.67},
	{1002175, ["weight"] = 0.67}, {1002176, ["weight"] = 0.57},
	{1002177, ["weight"] = 0.57}, {1002178, ["weight"] = 0.57},
	{1002179, ["weight"] = 0.57}, {1002180, ["weight"] = 0.57},
	{1002181, ["weight"] = 0.50}, {1002182, ["weight"] = 0.50},
	{1002183, ["weight"] = 0.50}, {1002184, ["weight"] = 0.50},
	{1002185, ["weight"] = 0.50}, {1002207, ["weight"] = 0.40},
	{1002208, ["weight"] = 0.40}, {1002209, ["weight"] = 0.40},
	{1002210, ["weight"] = 0.40}, {1002247, ["weight"] = 0.33},
	{1002248, ["weight"] = 0.33}, {1002249, ["weight"] = 0.33},
	{1002281, ["weight"] = 0.29}, {1002282, ["weight"] = 0.29},
	{1002283, ["weight"] = 0.29}, {1002284, ["weight"] = 0.29},
	{1002285, ["weight"] = 0.29}, {1002323, ["weight"] = 0.22},
	{1002324, ["weight"] = 0.22}, {1002325, ["weight"] = 0.22},
	{1002326, ["weight"] = 0.22}, {1002327, ["weight"] = 0.25},
	{1002328, ["weight"] = 0.25}, {1002329, ["weight"] = 0.25},
	{1002330, ["weight"] = 0.25}, {1002380, ["weight"] = 0.20},
	{1002381, ["weight"] = 0.20}, {1002382, ["weight"] = 0.20},
	{1002383, ["weight"] = 0.20}, {1002550, ["weight"] = 0.18},
	{1002577, ["weight"] = 0.50}, {1002656, ["weight"] = 0.33},
	{1002750, ["weight"] = 0.40}, {1002779, ["weight"] = 0.17},
	{1002793, ["weight"] = 0.17}, {1040031, ["weight"] = 2.00},
	{1040032, ["weight"] = 2.00}, {1040033, ["weight"] = 2.00},
	{1040034, ["weight"] = 1.33}, {1040035, ["weight"] = 1.33},
	{1040042, ["weight"] = 1.00}, {1040043, ["weight"] = 1.00},
	{1040044, ["weight"] = 1.00}, {1040048, ["weight"] = 0.80},
	{1040049, ["weight"] = 0.80}, {1040050, ["weight"] = 0.80},
	{1040057, ["weight"] = 0.67}, {1040058, ["weight"] = 0.67},
	{1040059, ["weight"] = 0.67}, {1040060, ["weight"] = 0.67},
	{1040061, ["weight"] = 0.57}, {1040062, ["weight"] = 0.57},
	{1040063, ["weight"] = 0.57}, {1040082, ["weight"] = 0.50},
	{1040083, ["weight"] = 0.50}, {1040084, ["weight"] = 0.50},
	{1040094, ["weight"] = 0.40}, {1040095, ["weight"] = 0.40},
	{1040096, ["weight"] = 0.40}, {1040097, ["weight"] = 0.40},
	{1040098, ["weight"] = 0.33}, {1040099, ["weight"] = 0.33},
	{1040100, ["weight"] = 0.33}, {1040105, ["weight"] = 0.29},
	{1040106, ["weight"] = 0.29}, {1040107, ["weight"] = 0.29},
	{1040108, ["weight"] = 0.25}, {1040109, ["weight"] = 0.25},
	{1040110, ["weight"] = 0.25}, {1040115, ["weight"] = 0.22},
	{1040116, ["weight"] = 0.22}, {1040117, ["weight"] = 0.22},
	{1040118, ["weight"] = 0.22}, {1041003, ["weight"] = 1.00},
	{1041036, ["weight"] = 2.00}, {1041037, ["weight"] = 2.00},
	{1041038, ["weight"] = 2.00}, {1041039, ["weight"] = 1.00},
	{1041040, ["weight"] = 1.00}, {1041044, ["weight"] = 1.33},
	{1041045, ["weight"] = 1.33}, {1041047, ["weight"] = 0.67},
	{1041048, ["weight"] = 0.67}, {1041049, ["weight"] = 0.67},
	{1041050, ["weight"] = 0.67}, {1041057, ["weight"] = 0.80},
	{1041058, ["weight"] = 0.80}, {1041059, ["weight"] = 0.80},
	{1041060, ["weight"] = 0.80}, {1041074, ["weight"] = 0.50},
	{1041075, ["weight"] = 0.50}, {1041076, ["weight"] = 0.50},
	{1041077, ["weight"] = 0.40}, {1041078, ["weight"] = 0.40},
	{1041079, ["weight"] = 0.40}, {1041080, ["weight"] = 0.40},
	{1041094, ["weight"] = 0.33}, {1041095, ["weight"] = 0.33},
	{1041096, ["weight"] = 0.33}, {1041100, ["weight"] = 0.29},
	{1041101, ["weight"] = 0.29}, {1041102, ["weight"] = 0.29},
	{1041103, ["weight"] = 0.29}, {1041105, ["weight"] = 0.25},
	{1041106, ["weight"] = 0.25}, {1041107, ["weight"] = 0.25},
	{1041115, ["weight"] = 0.22}, {1041116, ["weight"] = 0.22},
	{1041117, ["weight"] = 0.22}, {1041118, ["weight"] = 0.22},
	{1050096, ["weight"] = 0.20}, {1050097, ["weight"] = 0.20},
	{1050098, ["weight"] = 0.20}, {1050099, ["weight"] = 0.20},
	{1051006, ["weight"] = 0.57}, {1051007, ["weight"] = 0.57},
	{1051008, ["weight"] = 0.57}, {1051009, ["weight"] = 0.57},
	{1051090, ["weight"] = 0.20}, {1051091, ["weight"] = 0.20},
	{1051092, ["weight"] = 0.20}, {1051093, ["weight"] = 0.20},
	{1052072, ["weight"] = 0.18}, {1052158, ["weight"] = 0.17},
	{1052163, ["weight"] = 0.17}, {1060021, ["weight"] = 2.00},
	{1060022, ["weight"] = 2.00}, {1060023, ["weight"] = 2.00},
	{1060024, ["weight"] = 1.33}, {1060025, ["weight"] = 1.33},
	{1060031, ["weight"] = 1.00}, {1060032, ["weight"] = 1.00},
	{1060033, ["weight"] = 1.00}, {1060037, ["weight"] = 0.80},
	{1060038, ["weight"] = 0.80}, {1060039, ["weight"] = 0.80},
	{1060043, ["weight"] = 0.67}, {1060044, ["weight"] = 0.67},
	{1060045, ["weight"] = 0.67}, {1060046, ["weight"] = 0.67},
	{1060050, ["weight"] = 0.57}, {1060051, ["weight"] = 0.57},
	{1060052, ["weight"] = 0.57}, {1060071, ["weight"] = 0.50},
	{1060072, ["weight"] = 0.50}, {1060073, ["weight"] = 0.50},
	{1060083, ["weight"] = 0.40}, {1060084, ["weight"] = 0.40},
	{1060085, ["weight"] = 0.40}, {1060086, ["weight"] = 0.40},
	{1060087, ["weight"] = 0.33}, {1060088, ["weight"] = 0.33},
	{1060089, ["weight"] = 0.33}, {1060093, ["weight"] = 0.29},
	{1060094, ["weight"] = 0.29}, {1060095, ["weight"] = 0.29},
	{1060097, ["weight"] = 0.25}, {1060098, ["weight"] = 0.25},
	{1060099, ["weight"] = 0.25}, {1060104, ["weight"] = 0.22},
	{1060105, ["weight"] = 0.22}, {1060106, ["weight"] = 0.22},
	{1060107, ["weight"] = 0.22}, {1061003, ["weight"] = 1.00},
	{1061029, ["weight"] = 2.00}, {1061030, ["weight"] = 2.00},
	{1061031, ["weight"] = 2.00}, {1061032, ["weight"] = 1.00},
	{1061033, ["weight"] = 1.00}, {1061037, ["weight"] = 1.33},
	{1061038, ["weight"] = 1.33}, {1061040, ["weight"] = 0.91},
	{1061041, ["weight"] = 0.91}, {1061042, ["weight"] = 0.91},
	{1061043, ["weight"] = 0.67}, {1061044, ["weight"] = 0.67},
	{1061045, ["weight"] = 0.67}, {1061046, ["weight"] = 0.67},
	{1061053, ["weight"] = 0.80}, {1061054, ["weight"] = 0.80},
	{1061055, ["weight"] = 0.80}, {1061056, ["weight"] = 0.80},
	{1061069, ["weight"] = 0.50}, {1061070, ["weight"] = 0.50},
	{1061071, ["weight"] = 0.50}, {1061076, ["weight"] = 0.40},
	{1061077, ["weight"] = 0.40}, {1061078, ["weight"] = 0.40},
	{1061079, ["weight"] = 0.40}, {1061093, ["weight"] = 0.33},
	{1061094, ["weight"] = 0.33}, {1061095, ["weight"] = 0.33},
	{1061099, ["weight"] = 0.29}, {1061100, ["weight"] = 0.29},
	{1061101, ["weight"] = 0.29}, {1061102, ["weight"] = 0.29},
	{1061104, ["weight"] = 0.25}, {1061105, ["weight"] = 0.25},
	{1061106, ["weight"] = 0.25}, {1061114, ["weight"] = 0.22},
	{1061115, ["weight"] = 0.22}, {1061116, ["weight"] = 0.22},
	{1061117, ["weight"] = 0.22}, {1072022, ["weight"] = 1.00},
	{1072028, ["weight"] = 1.33}, {1072029, ["weight"] = 1.33},
	{1072030, ["weight"] = 1.33}, {1072031, ["weight"] = 1.33},
	{1072032, ["weight"] = 0.67}, {1072033, ["weight"] = 0.67},
	{1072035, ["weight"] = 0.67}, {1072036, ["weight"] = 0.67},
	{1072065, ["weight"] = 1.00}, {1072066, ["weight"] = 1.00},
	{1072070, ["weight"] = 2.00}, {1072071, ["weight"] = 2.00},
	{1072084, ["weight"] = 0.80}, {1072085, ["weight"] = 0.80},
	{1072086, ["weight"] = 0.80}, {1072087, ["weight"] = 0.80},
	{1072104, ["weight"] = 0.57}, {1072105, ["weight"] = 0.57},
	{1072106, ["weight"] = 0.57}, {1072107, ["weight"] = 0.50},
	{1072108, ["weight"] = 0.50}, {1072109, ["weight"] = 0.50},
	{1072110, ["weight"] = 0.50}, {1072128, ["weight"] = 0.40},
	{1072129, ["weight"] = 0.40}, {1072130, ["weight"] = 0.40},
	{1072131, ["weight"] = 0.40}, {1072150, ["weight"] = 0.33},
	{1072151, ["weight"] = 0.33}, {1072152, ["weight"] = 0.33},
	{1072161, ["weight"] = 0.29}, {1072162, ["weight"] = 0.29},
	{1072163, ["weight"] = 0.29}, {1072171, ["weight"] = 0.67},
	{1072172, ["weight"] = 0.25}, {1072173, ["weight"] = 0.25},
	{1072174, ["weight"] = 0.25}, {1072192, ["weight"] = 0.22},
	{1072193, ["weight"] = 0.22}, {1072194, ["weight"] = 0.22},
	{1072195, ["weight"] = 0.22}, {1072213, ["weight"] = 0.20},
	{1072214, ["weight"] = 0.20}, {1072215, ["weight"] = 0.20},
	{1072216, ["weight"] = 0.20}, {1072272, ["weight"] = 0.18},
	{1072346, ["weight"] = 0.40}, {1072358, ["weight"] = 0.17},
	{1072364, ["weight"] = 0.17}, {1082029, ["weight"] = 1.33},
	{1082030, ["weight"] = 1.33}, {1082031, ["weight"] = 1.33},
	{1082032, ["weight"] = 1.00}, {1082033, ["weight"] = 1.00},
	{1082034, ["weight"] = 1.00}, {1082037, ["weight"] = 0.80},
	{1082038, ["weight"] = 0.80}, {1082039, ["weight"] = 0.80},
	{1082042, ["weight"] = 0.67}, {1082043, ["weight"] = 0.67},
	{1082044, ["weight"] = 0.67}, {1082045, ["weight"] = 0.57},
	{1082046, ["weight"] = 0.57}, {1082047, ["weight"] = 0.57},
	{1082065, ["weight"] = 0.40}, {1082066, ["weight"] = 0.40},
	{1082067, ["weight"] = 0.40}, {1082074, ["weight"] = 0.50},
	{1082075, ["weight"] = 0.50}, {1082076, ["weight"] = 0.50},
	{1082092, ["weight"] = 0.33}, {1082093, ["weight"] = 0.33},
	{1082094, ["weight"] = 0.33}, {1082095, ["weight"] = 0.29},
	{1082096, ["weight"] = 0.29}, {1082097, ["weight"] = 0.29},
	{1082118, ["weight"] = 0.25}, {1082119, ["weight"] = 0.25},
	{1082120, ["weight"] = 0.25}, {1082135, ["weight"] = 0.20},
	{1082136, ["weight"] = 0.20}, {1082137, ["weight"] = 0.20},
	{1082138, ["weight"] = 0.20}, {1082142, ["weight"] = 0.22},
	{1082143, ["weight"] = 0.22}, {1082144, ["weight"] = 0.22},
	{1082167, ["weight"] = 0.18}, {1082237, ["weight"] = 0.17},
	{1082242, ["weight"] = 0.17}, {1092018, ["weight"] = 0.91},
	{1092019, ["weight"] = 0.91}, {1092020, ["weight"] = 0.91},
	{1092049, ["weight"] = 0.17}, {1092050, ["weight"] = 0.25},
	{1092059, ["weight"] = 0.17}, {1302001, ["weight"] = 2.00},
	{1312002, ["weight"] = 1.33}, {1332000, ["weight"] = 1.67},
	{1332001, ["weight"] = 0.63}, {1332002, ["weight"] = 1.18},
	{1332003, ["weight"] = 0.40}, {1332004, ["weight"] = 0.74},
	{1332011, ["weight"] = 0.50}, {1332012, ["weight"] = 0.67},
	{1332013, ["weight"] = 0.91}, {1332014, ["weight"] = 0.57},
	{1332015, ["weight"] = 0.33}, {1332018, ["weight"] = 0.29},
	{1332023, ["weight"] = 0.25}, {1332024, ["weight"] = 0.36},
	{1332027, ["weight"] = 0.22}, {1332031, ["weight"] = 0.50},
	{1332050, ["weight"] = 0.18}, {1332052, ["weight"] = 0.20},
	{1332054, ["weight"] = 0.40}, {1332064, ["weight"] = 0.29},
	{1332067, ["weight"] = 2.00}, {1332068, ["weight"] = 0.50},
	{1332069, ["weight"] = 0.25}, {1332070, ["weight"] = 2.00},
	{1332071, ["weight"] = 0.50}, {1332072, ["weight"] = 0.25},
	{1332074, ["weight"] = 0.17}, {1332076, ["weight"] = 0.17},
	{1332077, ["weight"] = 0.25}, {1332078, ["weight"] = 0.22},
	{1332079, ["weight"] = 0.22}, {1332080, ["weight"] = 0.25},
	{1472000, ["weight"] = 2.00}, {1472001, ["weight"] = 1.33},
	{1472002, ["weight"] = 1.33}, {1472003, ["weight"] = 1.33},
	{1472004, ["weight"] = 1.00}, {1472005, ["weight"] = 1.00},
	{1472006, ["weight"] = 1.00}, {1472007, ["weight"] = 0.80},
	{1472008, ["weight"] = 0.67}, {1472009, ["weight"] = 0.67},
	{1472010, ["weight"] = 0.67}, {1472011, ["weight"] = 0.57},
	{1472012, ["weight"] = 0.57}, {1472013, ["weight"] = 0.57},
	{1472014, ["weight"] = 0.50}, {1472015, ["weight"] = 0.50},
	{1472016, ["weight"] = 0.50}, {1472017, ["weight"] = 0.50},
	{1472018, ["weight"] = 0.40}, {1472019, ["weight"] = 0.40},
	{1472020, ["weight"] = 0.40}, {1472021, ["weight"] = 0.40},
	{1472022, ["weight"] = 0.33}, {1472023, ["weight"] = 0.33},
	{1472024, ["weight"] = 0.33}, {1472025, ["weight"] = 0.33},
	{1472026, ["weight"] = 0.29}, {1472027, ["weight"] = 0.29},
	{1472028, ["weight"] = 0.29}, {1472029, ["weight"] = 0.29},
	{1472031, ["weight"] = 0.25}, {1472033, ["weight"] = 0.22},
	{1472051, ["weight"] = 0.18}, {1472052, ["weight"] = 0.18},
	{1472053, ["weight"] = 0.20}, {1472054, ["weight"] = 0.36},
	{1472062, ["weight"] = 0.29}, {1472068, ["weight"] = 0.17},
	{1472069, ["weight"] = 0.50}, {1472071, ["weight"] = 0.17},
	{1472072, ["weight"] = 0.25}, {1472073, ["weight"] = 0.22},
	{1472074, ["weight"] = 0.22}, {1472075, ["weight"] = 0.25},
};

pirateEquips = {
	{1082192, ["weight"] = 0.29}, {1072288, ["weight"] = 0.50},
	{1492003, ["weight"] = 0.40}, {1052113, ["weight"] = 0.25},
	{1052104, ["weight"] = 0.40}, {1492002, ["weight"] = 0.50},
	{1052095, ["weight"] = 1.00}, {1492001, ["weight"] = 0.67},
	{1002613, ["weight"] = 0.67}, {1492004, ["weight"] = 0.33},
};

scrolls = {
	2044700, 2044701, 2044702, 2044704, 2044705, 2043300, 2043301, 2043302, 2043304, 2043305,
};

stars = {
	-- TODO FIXME gachapon
};

skills = {
	{2290076, ["weight"] = mastery_book_20}, {2290077, ["weight"] = mastery_book_30},
	{2290078, ["weight"] = mastery_book_20}, {2290079, ["weight"] = mastery_book_30},
	{2290080, ["weight"] = mastery_book_20}, {2290081, ["weight"] = mastery_book_30},
	{2290082, ["weight"] = mastery_book_20}, {2290083, ["weight"] = mastery_book_30},
	{2290084, ["weight"] = mastery_book_20}, {2290085, ["weight"] = mastery_book_30},
	{2290086, ["weight"] = mastery_book_20}, {2290087, ["weight"] = mastery_book_30},
	{2290088, ["weight"] = mastery_book_20}, {2290089, ["weight"] = mastery_book_30},
	{2290090, ["weight"] = mastery_book_20}, {2290091, ["weight"] = mastery_book_30},
	{2290092, ["weight"] = mastery_book_20}, {2290093, ["weight"] = mastery_book_30},
	{2290094, ["weight"] = mastery_book_20}, {2290095, ["weight"] = mastery_book_30},
};

items = merge(commonEquips, warriorEquips, magicianEquips, bowmanEquips, thiefEquips, pirateEquips, scrolls, stars, skills);

gachapon({
	["items"] = items,
	-- Decrease the chance of getting things from the global item list
	["globalItemWeightModifier"] = .5,
});