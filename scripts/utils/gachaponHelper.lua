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
-- Gachapon machines

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

item_keep = nil;
item_discard = 1;

function internalPreprocessItems(items)
	local finalized = {};
	for i = 1, #items do
		local item = items[i];
		if type(item) == "number" then
			if isValidItem(item) then
				item = {["id"] = item, ["weight"] = 1, ["qty"] = {["minimum"] = 1, ["maximum"] = 1}};
				append(finalized, item);
			end
		else
			local itemId = item[1];
			if isValidItem(itemId) then
				local qty = item["qty"];
				if qty == nil then
					qty = 1;
				end
				if type(qty) == "table" then
					local minimum = qty["minimum"];
					local maximum = qty["maximum"];
					if minimum == nil then
						minimum = 1;
					end
					if maximum == nil then
						maximum = getMaxStackSize(item["id"]);
					end
				else
					item["qty"] = {["minimum"] = qty, ["maximum"] = qty};
				end

				local weight = item["weight"];
				if weight == nil then
					weight = 1;
				end

				item["id"] = itemId;
				item["weight"] = weight;
				append(finalized, item);
			end
		end
	end

	return finalized;
end

local gachCommonEquipGloves = {
	1082002, 1082147, 1082148, 1082146, 1082145, 1082150, {1082149, ["weight"] = .80},
};

local gachCommonEquipCapes = {
	{1102041, ["weight"] = .70}, {1102042, ["weight"] = .70}, {1102040, ["weight"] = .80},
	{1102084, ["weight"] = .50}, {1102085, ["weight"] = .50}, {1102084, ["weight"] = .60},
	1102043, 1102087, 1102079, 1102080, 1102081, 1102082, 1102083, 1102053, 1102054, 1102055,
	1102056, 1102176, 1102177, 1102178, 1102179, 1102180, 1102000, 1102001, 1102002, 1102003,
	1102004, 1102011, 1102012, 1102013, 1102014, 1102015, 1102016, 1102017, 1102018, 1102021,
	1102022, 1102023, 1102024, 1102021, 1102021, 1102021, 1102021, 1102021, 1102021, 1102021,
};

local gachCommonEquipTubes = {
	1442018, 1322026, 1322025, 1322024, 1322022, 1322021,
};

local gachCommonEquipUmbrellas = {
	1302026, 1302027, 1302028, 1302017,
};

local gachCommonEquipSnowboards = {
	1442017, 1442016, 1442014, 1442012,
};

local gachCommonEquipSkis = {
	1432018, 1432017, 1432016, 1432015,
};

local gachCommonEquips = merge(gachCommonEquipGloves, gachCommonEquipCapes, gachCommonEquipTubes, gachCommonEquipUmbrellas, gachCommonEquipSnowboards, gachCommonEquipSkis);

local gachBeginnerEquips = {
	1442018, 1422011,
};

local gachMapleEquips = {
	{1302020, ["weight"] = .10}, {1302030, ["weight"] = .07},
	-- 1h Axe (131xxxx) has no 35/43 Maple equips
	-- 1h BW (132xxxx) has no 35/43 Maple equips
	{1332025, ["weight"] = .07}, -- Dagger (133xxxx) has no 35 Maple equip
	-- Wand (137xxxx) has no 35/43 Maple equips
	{1382009, ["weight"] = .10}, {1382012, ["weight"] = .07},
	-- 2h Sword (140xxxx) has no 35/43 Maple equips
	{1412011, ["weight"] = .07}, -- 2h Axe (141xxxx) has no 35 Maple equip
	{1422014, ["weight"] = .07}, -- 2h BW (142xxxx) has no 35 Maple equip
	{1432012, ["weight"] = .07}, -- Spear (143xxxx) has no 35 Maple equip
	{1442024, ["weight"] = .07}, -- Polearm (144xxxx) has no 35 Maple equip
	{1452016, ["weight"] = .10}, {1452022, ["weight"] = .07},
	{1462014, ["weight"] = .10}, {1462019, ["weight"] = .07},
	{1472030, ["weight"] = .10}, {1472032, ["weight"] = .07},
	{1482020, ["weight"] = .10}, {1482021, ["weight"] = .07},
	{1492020, ["weight"] = .10}, {1492021, ["weight"] = .07},
};

local gachPotions = {
	{2000004, ["weight"] = .60, ["qty"] = 100}, {2000005, ["weight"] = .60, ["qty"] = 100},
	{2001002, ["weight"] = .60, ["qty"] = 100}, {2020012, ["weight"] = .60, ["qty"] = 100},
	{2001001, ["weight"] = .60, ["qty"] = 100}, {2020013, ["weight"] = .60, ["qty"] = 100},
	{2020014, ["weight"] = .60, ["qty"] = 100}, {2020015, ["weight"] = .60, ["qty"] = 100},
	2022179, 2022182, 2022245, 2022273, 2022284, 2022282, 2022283, 2022285, 2022439,
};

local gachMiscScrolls = {
	2049100, 2049000, 2049001, 2049002, 2049003, 2041058, 2040727,
};

local gachTenPercentScrolls = {
	2041002, 2040402, 2040702, 2040805, 2040026, 2040031, 2040318, 2040323, 2040328, 2040419,
	2040422, 2040427, 2040534, 2040619, 2040622, 2040627, 2040825, 2040925, 2040928, 2040933,
	2040016,
};

local gachSixtyPercentScrolls = {
	{2040001, ["weight"] = 2}, {2040025, ["weight"] = 2}, {2040029, ["weight"] = 2}, {2040317, ["weight"] = 2},
	{2040321, ["weight"] = 2}, {2040326, ["weight"] = 2}, {2040418, ["weight"] = 2}, {2040421, ["weight"] = 2},
	{2040425, ["weight"] = 2}, {2040532, ["weight"] = 2}, {2040618, ["weight"] = 2}, {2040621, ["weight"] = 2},
	{2040625, ["weight"] = 2}, {2040824, ["weight"] = 2}, {2040924, ["weight"] = 2}, {2040927, ["weight"] = 2},
	{2040931, ["weight"] = 2}, {2048010, ["weight"] = 2}, {2048011, ["weight"] = 2}, {2048012, ["weight"] = 2},
	{8048013, ["weight"] = 2},
};

local gachHundredPercentScrolls = {
	{2040923, ["weight"] = .5}, {2040926, ["weight"] = .5}, {2040929, ["weight"] = .5}, {2040024, ["weight"] = .5},
	{2040027, ["weight"] = .5}, {2040316, ["weight"] = .5}, {2040319, ["weight"] = .5}, {2040324, ["weight"] = .5},
	{2040417, ["weight"] = .5}, {2040420, ["weight"] = .5}, {2040423, ["weight"] = .5}, {2040530, ["weight"] = .5},
	{2040617, ["weight"] = .5}, {2040620, ["weight"] = .5}, {2040623, ["weight"] = .5}, {2040823, ["weight"] = .5},
	{2040000, ["weight"] = .5}, {2040003, ["weight"] = .5}, {2040102, ["weight"] = .5}, {2040107, ["weight"] = .5},
	{2040202, ["weight"] = .5}, {2040207, ["weight"] = .5}, {2040300, ["weight"] = .5}, {2040312, ["weight"] = .5},
	{2040400, ["weight"] = .5}, {2040414, ["weight"] = .5}, {2040500, ["weight"] = .5}, {2040503, ["weight"] = .5},
	{2040512, ["weight"] = .5}, {2040515, ["weight"] = .5}, {2040600, ["weight"] = .5}, {2040614, ["weight"] = .5},
	{2040700, ["weight"] = .5}, {2040703, ["weight"] = .5}, {2040706, ["weight"] = .5}, {2040800, ["weight"] = .5},
	{2040803, ["weight"] = .5}, {2040818, ["weight"] = .5}, {2040900, ["weight"] = .5}, {2040918, ["weight"] = .5},
	{2041000, ["weight"] = .5}, {2041003, ["weight"] = .5}, {2041006, ["weight"] = .5}, {2041009, ["weight"] = .5},
	{2041012, ["weight"] = .5}, {2041015, ["weight"] = .5}, {2041018, ["weight"] = .5}, {2041021, ["weight"] = .5},
	{2048000, ["weight"] = .5}, {2048003, ["weight"] = .5},
};

local gachScrolls = merge(gachHundredPercentScrolls, gachSixtyPercentScrolls, gachTenPercentScrolls, gachMiscScrolls);

mastery_book_20 = .05;
mastery_book_30 = .025;

local gachSkills = {
	{2290096, ["weight"] = mastery_book_20}, {2290125, ["weight"] = mastery_book_30},
};

local gachGlobalGachaponItems = internalPreprocessItems(merge(gachPotions, gachScrolls, gachSkills, gachBeginnerEquips, gachMapleEquips, gachCommonEquips));

function gachapon(args)
	local items = args["items"];
	local gachItem = args["gachItem"];
	local globalItemModifier = args["globalItemWeightModifier"];

	if gachItem == nil then
		gachItem = 5220000;
	end
	if items == nil then
		items = {};
	end
	if type(globalItemModifier) == "function" then
		local finalized = {};
		for i = 1, #gachGlobalGachaponItems do
			local item = gachGlobalGachaponItems[i];
			if globalItemModifier(item) == item_keep then
				append(finalized, item);
			end
		end
		gachGlobalGachaponItems = finalized;
	elseif type(globalItemModifier) == "number" then
		for i = 1, #gachGlobalGachaponItems do
			local item = gachGlobalGachaponItems[i];
			item["weight"] = item["weight"] * globalItemModifier;
		end
	end

	items = merge(internalPreprocessItems(items), gachGlobalGachaponItems);
	local weights = {};
	for i = 1, #items do
		local item = items[i];
		append(weights, item["weight"]);
	end

	if getLevel() < 15 then
		addText("You need to be at least Level 15 in order to use Gachapon.");
		sendNext();
	elseif getItemAmount(gachItem) >= 1 then
		addText("You have a " .. blue(itemRef(gachItem)) .. ". ");
		addText("Would you like to use it?");
		local answer = askYesNo();

		if answer == answer_yes then
			for i = 1, 4 do
				if getOpenSlots(i) == 0 then
					addText("Please make room on your item inventory and then try again.");
					sendNext();
					return;
				end
			end

			local itemIndex = selectDiscrete(weights);
			local item = items[itemIndex];
			local maximum = item["qty"]["maximum"];
			local minimum = item["qty"]["minimum"];
			local id = item["id"];

			if minimum == maximum then
				qty = minimum;
			else
				qty = (getRandomNumber(maximum - minimum + 1) - 1) + minimum;
			end

			giveItem(gachItem, -1);
			giveItemGachapon(id, qty);

			addText("You have obtained " .. blue(itemRef(id)) .. ".");
			sendNext();
		else
			addText("Please come again!");
			sendNext();
		end
	else
		addText("Here's Gachapon.");
		sendOk();
	end
end