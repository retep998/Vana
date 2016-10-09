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
-- Gachapon - Zipangu : Mushroom Shrine

dofile("scripts/utils/gachaponHelper.lua");

-- General rules:
-- Only scrolls from the global items and 90% weight on the global scrolls
-- Includes "local" items (e.g. potions and equipment you get from Zipangu, Showa, etc.)
-- Includes scrolls for all weapons

equips = {
	1382011, 1332024, 1302022, 1302021, 1462006, 1402009, 1402010, 1322012, 1312013, 1472008,
	1432008, 1050100, 1051098,
};

useItems = {
	2070000, 2030008, 2030009, 2030010,
	{2022002, ["qty"] = getMaxStackSize(2022002)},
	{2020011, ["qty"] = getMaxStackSize(2020011)},
	{2022014, ["qty"] = getMaxStackSize(2022014)},
	{2022015, ["qty"] = getMaxStackSize(2022015)},
	{2022017, ["qty"] = getMaxStackSize(2022017)},
	{2022018, ["qty"] = getMaxStackSize(2022018)},
	{2022019, ["qty"] = getMaxStackSize(2022019)},
	{2022020, ["qty"] = getMaxStackSize(2022020)},
	{2022021, ["qty"] = getMaxStackSize(2022021)},
	{2022023, ["qty"] = getMaxStackSize(2022023)},
	{2022022, ["qty"] = getMaxStackSize(2022022)},
	{2022024, ["qty"] = getMaxStackSize(2022024)},
	{2022025, ["qty"] = getMaxStackSize(2022025)},
	{2022026, ["qty"] = getMaxStackSize(2022026)},
	{2022027, ["qty"] = getMaxStackSize(2022027)},
};

scrolls = {
	2043015, 2043017, 2043019, 2044000, 2044001, 2044002, 2044004, 2044005, 2044010, 2044012,
	2044014, 2043200, 2043201, 2043202, 2043204, 2043205, 2043210, 2043212, 2043214, 2044200,
	2044201, 2044202, 2044204, 2044205, 2044210, 2044212, 2044214, 2044300, 2044301, 2044302,
	2044304, 2044305, 2044310, 2044312, 2044314, 2044400, 2044401, 2044402, 2044404, 2044405,
	2044410, 2044412, 2044414,

	2043800, 2043801, 2043802, 2043804, 2043805, 2043700, 2043701, 2043702, 2043704, 2043705,

	2044605, 2044604, 2044602, 2044601, 2044600, 2044505, 2044504, 2044502, 2044501, 2044500,

	2044700, 2044701, 2044702, 2044704, 2044705, 2043300, 2043301, 2043302, 2043304, 2043305,

	2044800, 2044801, 2044802, 2044803, 2044804, 2044805, 2044807, 2044809, 2044900, 2044901,
	2044902, 2044903, 2044904,
};

etc = {
	4000138,
};

items = merge(equips, useItems, scrolls, etc);

gachapon({
	["items"] = items,
	["globalItemWeightModifier"] = function(item)
		-- Only pass scrolls through
		if math.floor(item["id"] / 10000) == 204 then
			item["weight"] = item["weight"] * .9;
			return item_keep;
		end
		return item_discard;
	end,
});