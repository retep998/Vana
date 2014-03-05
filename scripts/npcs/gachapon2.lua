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
dofile("scripts/lua_functions/npcHelper.lua");

items = {
	2043302, 2040002, 2043102, 2043002, 2044402, 2044302, 2043802, 2044002, 2041017, 2000004,
	2000005, 2022025, 2022026, 1402010, 1442013, 1432009, 1002060, 1002063, 1322023, 1002042,
	1050018, 1082147, 1002026, 1002392, 1082149, 1060005, 1442016, 1322025, 1322027, 1002391,
	1302027, 1372006, 1002419, 1302019, 1092022, 1302021, 1102041, 1041004, 1002395, 1322024,
	1082148, 1002012, 1322012, 1032028, 1102012, 1322022, 1051017, 1302013, 1082146, 1442014,
	1302017, 1102013, 1102003, 1002041, 1002097, 1302016, 1082145, 1412006, 1040029, 1040086,
	1050005, 1060028, 1002059, 1060008, 1061088, 1402012, 1302003, 1432002, 1312011, 1302008,
	1442008, 1002004, 1402015, 1322028, 1322015, 1432006, 1442006, 1322000, 1002085, 1002056,
	1092013, 1002058, 1002050, 1060011, 1322009, 1322011, 1442000, 1051011, 1041019, 1060018,
	1061020, 1041024, 1302005, 1402002, 1002030, 1092004, 1041023, 1422008, 1060009, 1051000,
	1002021, 1442005, 1412003, 1412007, 1422007, 1302009, 1402000, 1402001, 1402007, 1432005,
	1382001, 1002037, 1060014, 1040018, 1061027, 1050002, 1002152, 1051027, 1050035, 1050056,
	1051047, 1051030, 1002274, 1050074, 1002218, 1002254, 1082088, 1382007, 1002013, 1082087,
	1372008, 1382008, 1372002, 1372003, 1382011, 1382004, 1050047, 1040019, 1041041, 1061034,
	1061047, 1051045, 1051024, 1082081, 1041030, 1040018, 1002073, 1382003, 1082086, 1382014,
	1050055, 1050025, 1002155, 1060015, 1452004, 1462003, 1060070, 1002118, 1061058, 1040003,
	1002160, 1002121, 1040068, 1061063, 1040080, 1462004, 1041008, 1061006, 1061009, 1040022,
	1002168, 1040067, 1060056, 1041054, 1041067, 1060063, 1002213, 1002119, 1462005, 1452001,
	1462000, 1040025, 1002166, 1002161, 1040069, 1051039, 1452006, 1462006, 1452007, 1402001,
	1041062, 1472010, 1472006, 1332011, 1472031, 1041048, 1472019, 1041095, 1040095, 1002128,
	1061077, 1060025, 1041040, 1061033, 1472028, 1472022, 1472011, 1040096, 1062002, 1002129,
	1472026, 1332009, 1060043, 1002249, 1472021, 1040084, 1332015, 1002173, 1002148, 1332004,
	1332018, 1472009, 1061069, 1002176, 1041044, 1061037, 1061003, 1472020, 1040060, 1472018,
	1332013, 1332002
};

gachapon(items);