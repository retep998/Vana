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
-- Gachapon - Victoria Road : Ellinia

dofile("scripts/lua_functions/gachaponHelper.lua");

-- General rules:
-- Global item chance is significantly reduced
-- All gear for town class; weight derived at 20 / itemLevel
-- 10 random gears between levels 10 and 40 from other town classes; weight derived at 10 / itemLevel
-- All skills for the town class
-- All weapon scrolls for the town class' weapons

commonEquips = {};

warriorEquips = {
	{1082025, ["weight"] = 0.25}, {1060028, ["weight"] = 1.00},
	{1051011, ["weight"] = 0.33}, {1060016, ["weight"] = 0.33},
	{1051001, ["weight"] = 0.29}, {1422001, ["weight"] = 0.33},
	{1002025, ["weight"] = 0.25}, {1082000, ["weight"] = 0.67},
	{1302002, ["weight"] = 0.50}, {1412012, ["weight"] = 0.67},
};

magicianEquips = {
	{1002013, ["weight"] = 0.67}, {1002016, ["weight"] = 1.00},
	{1002017, ["weight"] = 2.00}, {1002034, ["weight"] = 0.67},
	{1002035, ["weight"] = 0.67}, {1002036, ["weight"] = 0.67},
	{1002037, ["weight"] = 0.67}, {1002038, ["weight"] = 0.67},
	{1002064, ["weight"] = 0.67}, {1002065, ["weight"] = 0.67},
	{1002072, ["weight"] = 1.00}, {1002073, ["weight"] = 1.00},
	{1002074, ["weight"] = 2.00}, {1002075, ["weight"] = 2.00},
	{1002102, ["weight"] = 1.33}, {1002103, ["weight"] = 1.33},
	{1002104, ["weight"] = 1.33}, {1002105, ["weight"] = 1.33},
	{1002106, ["weight"] = 1.33}, {1002141, ["weight"] = 0.57},
	{1002142, ["weight"] = 0.57}, {1002143, ["weight"] = 0.57},
	{1002144, ["weight"] = 0.57}, {1002145, ["weight"] = 0.57},
	{1002151, ["weight"] = 0.50}, {1002152, ["weight"] = 0.50},
	{1002153, ["weight"] = 0.50}, {1002154, ["weight"] = 0.50},
	{1002155, ["weight"] = 0.50}, {1002215, ["weight"] = 0.40},
	{1002216, ["weight"] = 0.40}, {1002217, ["weight"] = 0.40},
	{1002218, ["weight"] = 0.40}, {1002242, ["weight"] = 0.33},
	{1002243, ["weight"] = 0.33}, {1002244, ["weight"] = 0.33},
	{1002245, ["weight"] = 0.33}, {1002246, ["weight"] = 0.33},
	{1002252, ["weight"] = 0.29}, {1002253, ["weight"] = 0.29},
	{1002254, ["weight"] = 0.29}, {1002271, ["weight"] = 0.25},
	{1002272, ["weight"] = 0.25}, {1002273, ["weight"] = 0.25},
	{1002274, ["weight"] = 0.25}, {1002363, ["weight"] = 0.22},
	{1002364, ["weight"] = 0.22}, {1002365, ["weight"] = 0.22},
	{1002366, ["weight"] = 0.22}, {1002398, ["weight"] = 0.20},
	{1002399, ["weight"] = 0.20}, {1002400, ["weight"] = 0.20},
	{1002401, ["weight"] = 0.20}, {1002579, ["weight"] = 0.67},
	{1002773, ["weight"] = 0.18}, {1002777, ["weight"] = 0.17},
	{1002791, ["weight"] = 0.17}, {1040004, ["weight"] = 2.50},
	{1040017, ["weight"] = 2.50}, {1040018, ["weight"] = 1.11},
	{1040019, ["weight"] = 1.11}, {1040020, ["weight"] = 1.11},
	{1041015, ["weight"] = 2.50}, {1041016, ["weight"] = 2.50},
	{1041017, ["weight"] = 1.54}, {1041018, ["weight"] = 1.54},
	{1041025, ["weight"] = 1.54}, {1041026, ["weight"] = 1.54},
	{1041029, ["weight"] = 1.11}, {1041030, ["weight"] = 1.11},
	{1041031, ["weight"] = 1.11}, {1041041, ["weight"] = 0.71},
	{1041042, ["weight"] = 0.71}, {1041043, ["weight"] = 0.71},
	{1041051, ["weight"] = 0.61}, {1041052, ["weight"] = 0.61},
	{1041053, ["weight"] = 0.61}, {1050001, ["weight"] = 0.87},
	{1050002, ["weight"] = 0.61}, {1050003, ["weight"] = 0.71},
	{1050008, ["weight"] = 1.54}, {1050009, ["weight"] = 1.54},
	{1050010, ["weight"] = 1.54}, {1050023, ["weight"] = 0.87},
	{1050024, ["weight"] = 0.87}, {1050025, ["weight"] = 0.87},
	{1050026, ["weight"] = 0.71}, {1050027, ["weight"] = 0.71},
	{1050028, ["weight"] = 0.71}, {1050029, ["weight"] = 0.61},
	{1050030, ["weight"] = 0.61}, {1050031, ["weight"] = 0.61},
	{1050035, ["weight"] = 0.53}, {1050036, ["weight"] = 0.53},
	{1050037, ["weight"] = 0.53}, {1050038, ["weight"] = 0.53},
	{1050039, ["weight"] = 0.53}, {1050045, ["weight"] = 0.42},
	{1050046, ["weight"] = 0.42}, {1050047, ["weight"] = 0.42},
	{1050048, ["weight"] = 0.42}, {1050049, ["weight"] = 0.42},
	{1050053, ["weight"] = 0.34}, {1050054, ["weight"] = 0.34},
	{1050055, ["weight"] = 0.34}, {1050056, ["weight"] = 0.34},
	{1050067, ["weight"] = 0.29}, {1050068, ["weight"] = 0.29},
	{1050069, ["weight"] = 0.29}, {1050070, ["weight"] = 0.29},
	{1050072, ["weight"] = 0.26}, {1050073, ["weight"] = 0.26},
	{1050074, ["weight"] = 0.26}, {1050092, ["weight"] = 0.23},
	{1050093, ["weight"] = 0.23}, {1050094, ["weight"] = 0.23},
	{1050095, ["weight"] = 0.23}, {1050102, ["weight"] = 0.20},
	{1050103, ["weight"] = 0.20}, {1050104, ["weight"] = 0.20},
	{1050105, ["weight"] = 0.20}, {1051003, ["weight"] = 0.87},
	{1051004, ["weight"] = 0.87}, {1051005, ["weight"] = 0.87},
	{1051023, ["weight"] = 0.53}, {1051024, ["weight"] = 0.53},
	{1051025, ["weight"] = 0.53}, {1051026, ["weight"] = 0.53},
	{1051027, ["weight"] = 0.53}, {1051030, ["weight"] = 0.42},
	{1051031, ["weight"] = 0.42}, {1051032, ["weight"] = 0.42},
	{1051033, ["weight"] = 0.42}, {1051034, ["weight"] = 0.42},
	{1051044, ["weight"] = 0.34}, {1051045, ["weight"] = 0.34},
	{1051046, ["weight"] = 0.34}, {1051047, ["weight"] = 0.34},
	{1051052, ["weight"] = 0.29}, {1051053, ["weight"] = 0.29},
	{1051054, ["weight"] = 0.29}, {1051055, ["weight"] = 0.29},
	{1051056, ["weight"] = 0.26}, {1051057, ["weight"] = 0.26},
	{1051058, ["weight"] = 0.26}, {1051094, ["weight"] = 0.23},
	{1051095, ["weight"] = 0.23}, {1051096, ["weight"] = 0.23},
	{1051097, ["weight"] = 0.23}, {1051101, ["weight"] = 0.20},
	{1051102, ["weight"] = 0.20}, {1051103, ["weight"] = 0.20},
	{1051104, ["weight"] = 0.20}, {1052076, ["weight"] = 0.19},
	{1052156, ["weight"] = 0.17}, {1052161, ["weight"] = 0.17},
	{1060012, ["weight"] = 2.50}, {1060013, ["weight"] = 2.50},
	{1060014, ["weight"] = 1.11}, {1060015, ["weight"] = 1.11},
	{1061010, ["weight"] = 2.50}, {1061011, ["weight"] = 2.50},
	{1061012, ["weight"] = 1.54}, {1061013, ["weight"] = 1.54},
	{1061021, ["weight"] = 1.54}, {1061022, ["weight"] = 1.54},
	{1061027, ["weight"] = 1.11}, {1061028, ["weight"] = 1.11},
	{1061034, ["weight"] = 0.71}, {1061035, ["weight"] = 0.71},
	{1061036, ["weight"] = 0.71}, {1061047, ["weight"] = 0.61},
	{1061048, ["weight"] = 0.61}, {1061049, ["weight"] = 0.61},
	{1072006, ["weight"] = 2.00}, {1072019, ["weight"] = 1.00},
	{1072020, ["weight"] = 1.00}, {1072021, ["weight"] = 1.00},
	{1072023, ["weight"] = 1.33}, {1072024, ["weight"] = 1.33},
	{1072044, ["weight"] = 2.00}, {1072045, ["weight"] = 2.00},
	{1072072, ["weight"] = 0.80}, {1072073, ["weight"] = 0.80},
	{1072074, ["weight"] = 0.80}, {1072075, ["weight"] = 0.67},
	{1072076, ["weight"] = 0.67}, {1072077, ["weight"] = 0.67},
	{1072078, ["weight"] = 0.67}, {1072089, ["weight"] = 0.57},
	{1072090, ["weight"] = 0.57}, {1072091, ["weight"] = 0.57},
	{1072114, ["weight"] = 0.50}, {1072115, ["weight"] = 0.50},
	{1072116, ["weight"] = 0.50}, {1072117, ["weight"] = 0.50},
	{1072136, ["weight"] = 0.33}, {1072137, ["weight"] = 0.33},
	{1072138, ["weight"] = 0.33}, {1072139, ["weight"] = 0.33},
	{1072140, ["weight"] = 0.40}, {1072141, ["weight"] = 0.40},
	{1072142, ["weight"] = 0.40}, {1072143, ["weight"] = 0.40},
	{1072157, ["weight"] = 0.29}, {1072158, ["weight"] = 0.29},
	{1072159, ["weight"] = 0.29}, {1072160, ["weight"] = 0.29},
	{1072169, ["weight"] = 0.67}, {1072177, ["weight"] = 0.25},
	{1072178, ["weight"] = 0.25}, {1072179, ["weight"] = 0.25},
	{1072206, ["weight"] = 0.22}, {1072207, ["weight"] = 0.22},
	{1072208, ["weight"] = 0.22}, {1072209, ["weight"] = 0.22},
	{1072223, ["weight"] = 0.20}, {1072224, ["weight"] = 0.20},
	{1072225, ["weight"] = 0.20}, {1072226, ["weight"] = 0.20},
	{1072268, ["weight"] = 0.18}, {1072356, ["weight"] = 0.17},
	{1072362, ["weight"] = 0.17}, {1082019, ["weight"] = 1.33},
	{1082020, ["weight"] = 1.00}, {1082021, ["weight"] = 1.00},
	{1082022, ["weight"] = 1.00}, {1082026, ["weight"] = 0.80},
	{1082027, ["weight"] = 0.80}, {1082028, ["weight"] = 0.80},
	{1082051, ["weight"] = 0.67}, {1082052, ["weight"] = 0.67},
	{1082053, ["weight"] = 0.67}, {1082054, ["weight"] = 0.57},
	{1082055, ["weight"] = 0.57}, {1082056, ["weight"] = 0.57},
	{1082062, ["weight"] = 0.50}, {1082063, ["weight"] = 0.50},
	{1082064, ["weight"] = 0.50}, {1082080, ["weight"] = 0.40},
	{1082081, ["weight"] = 0.40}, {1082082, ["weight"] = 0.40},
	{1082086, ["weight"] = 0.33}, {1082087, ["weight"] = 0.33},
	{1082088, ["weight"] = 0.33}, {1082098, ["weight"] = 0.29},
	{1082099, ["weight"] = 0.29}, {1082100, ["weight"] = 0.29},
	{1082121, ["weight"] = 0.25}, {1082122, ["weight"] = 0.25},
	{1082123, ["weight"] = 0.25}, {1082131, ["weight"] = 0.22},
	{1082132, ["weight"] = 0.22}, {1082133, ["weight"] = 0.22},
	{1082134, ["weight"] = 0.22}, {1082151, ["weight"] = 0.20},
	{1082152, ["weight"] = 0.20}, {1082153, ["weight"] = 0.20},
	{1082154, ["weight"] = 0.20}, {1082164, ["weight"] = 0.18},
	{1082235, ["weight"] = 0.17}, {1082240, ["weight"] = 0.17},
	{1092021, ["weight"] = 0.91}, {1092029, ["weight"] = 0.61},
	{1092057, ["weight"] = 0.17}, {1372000, ["weight"] = 0.53},
	{1372001, ["weight"] = 0.61}, {1372002, ["weight"] = 1.11},
	{1372003, ["weight"] = 0.71}, {1372004, ["weight"] = 0.87},
	{1372007, ["weight"] = 0.42}, {1372008, ["weight"] = 0.33},
	{1372009, ["weight"] = 0.23}, {1372010, ["weight"] = 0.20},
	{1372011, ["weight"] = 0.40}, {1372012, ["weight"] = 0.53},
	{1372014, ["weight"] = 0.34}, {1372015, ["weight"] = 0.29},
	{1372016, ["weight"] = 0.26}, {1372032, ["weight"] = 0.19},
	{1372035, ["weight"] = 0.29}, {1372036, ["weight"] = 0.29},
	{1372037, ["weight"] = 0.29}, {1372038, ["weight"] = 0.29},
	{1372039, ["weight"] = 0.15}, {1372040, ["weight"] = 0.15},
	{1372041, ["weight"] = 0.15}, {1372042, ["weight"] = 0.15},
	{1372044, ["weight"] = 0.17}, {1372045, ["weight"] = 0.17},
	{1382000, ["weight"] = 2.00}, {1382001, ["weight"] = 0.44},
	{1382002, ["weight"] = 0.80}, {1382003, ["weight"] = 1.33},
	{1382004, ["weight"] = 1.00}, {1382005, ["weight"] = 1.33},
	{1382006, ["weight"] = 0.36}, {1382007, ["weight"] = 0.31},
	{1382008, ["weight"] = 0.24}, {1382010, ["weight"] = 0.27},
	{1382011, ["weight"] = 0.39}, {1382014, ["weight"] = 0.38},
	{1382015, ["weight"] = 0.67}, {1382016, ["weight"] = 0.22},
	{1382017, ["weight"] = 0.67}, {1382018, ["weight"] = 0.57},
	{1382019, ["weight"] = 0.50}, {1382035, ["weight"] = 0.21},
	{1382036, ["weight"] = 0.18}, {1382037, ["weight"] = 0.20},
	{1382041, ["weight"] = 0.47}, {1382045, ["weight"] = 0.19},
	{1382046, ["weight"] = 0.19}, {1382047, ["weight"] = 0.19},
	{1382048, ["weight"] = 0.19}, {1382049, ["weight"] = 0.12},
	{1382050, ["weight"] = 0.12}, {1382051, ["weight"] = 0.12},
	{1382052, ["weight"] = 0.12}, {1382053, ["weight"] = 0.29},
	{1382054, ["weight"] = 2.00}, {1382055, ["weight"] = 0.50},
	{1382056, ["weight"] = 0.25}, {1382057, ["weight"] = 0.17},
	{1382059, ["weight"] = 0.17}, {1382060, ["weight"] = 0.20},
};

bowmanEquips = {
	{1040003, ["weight"] = 0.50}, {1462005, ["weight"] = 0.26},
	{1002157, ["weight"] = 0.40}, {1002119, ["weight"] = 0.50},
	{1062002, ["weight"] = 0.50}, {1062004, ["weight"] = 1.00},
	{1041083, ["weight"] = 0.25}, {1061082, ["weight"] = 0.25},
	{1082017, ["weight"] = 0.40}, {1041061, ["weight"] = 1.00},
};

thiefEquips = {
	{1002174, ["weight"] = 0.33}, {1040063, ["weight"] = 0.29},
	{1072108, ["weight"] = 0.25}, {1082044, ["weight"] = 0.33},
	{1061071, ["weight"] = 0.25}, {1060052, ["weight"] = 0.29},
	{1072029, ["weight"] = 0.67}, {1472002, ["weight"] = 0.67},
	{1082031, ["weight"] = 0.67}, {1060023, ["weight"] = 1.00},
};

pirateEquips = {
	{1002625, ["weight"] = 0.29}, {1002616, ["weight"] = 0.50},
	{1482005, ["weight"] = 0.29}, {1052098, ["weight"] = 0.67},
	{1482003, ["weight"] = 0.40}, {1482001, ["weight"] = 0.67},
	{1492004, ["weight"] = 0.33}, {1002622, ["weight"] = 0.33},
	{1492005, ["weight"] = 0.29}, {1082195, ["weight"] = 0.25},
};

scrolls = {
	2043800, 2043801, 2043802, 2043804, 2043805, 2043700, 2043701, 2043702, 2043704, 2043705,
};

skills = {
	{2290024, ["weight"] = mastery_book_20}, {2290025, ["weight"] = mastery_book_30},
	{2290026, ["weight"] = mastery_book_20}, {2290027, ["weight"] = mastery_book_30},
	{2290028, ["weight"] = mastery_book_20}, {2290029, ["weight"] = mastery_book_30},
	{2290030, ["weight"] = mastery_book_20}, {2290031, ["weight"] = mastery_book_30},
	{2290032, ["weight"] = mastery_book_20}, {2290033, ["weight"] = mastery_book_30},
	{2290034, ["weight"] = mastery_book_20}, {2290035, ["weight"] = mastery_book_30},
	{2290036, ["weight"] = mastery_book_20}, {2290037, ["weight"] = mastery_book_30},
	{2290038, ["weight"] = mastery_book_20}, {2290039, ["weight"] = mastery_book_30},
	{2290040, ["weight"] = mastery_book_20}, {2290041, ["weight"] = mastery_book_30},
	{2290042, ["weight"] = mastery_book_20}, {2290043, ["weight"] = mastery_book_30},
	{2290044, ["weight"] = mastery_book_20}, {2290045, ["weight"] = mastery_book_30},
	{2290046, ["weight"] = mastery_book_20}, {2290047, ["weight"] = mastery_book_30},
	{2290048, ["weight"] = mastery_book_20}, {2290049, ["weight"] = mastery_book_30},
	{2290050, ["weight"] = mastery_book_20}, {2290051, ["weight"] = mastery_book_30},
};

items = merge(commonEquips, warriorEquips, magicianEquips, bowmanEquips, thiefEquips, pirateEquips, scrolls, skills);

gachapon({
	["items"] = items,
	-- Decrease the chance of getting things from the global item list
	["globalItemWeightModifier"] = .5,
});