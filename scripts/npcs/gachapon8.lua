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
-- Gachapon - Zipangu: Spa (F)

dofile("scripts/utils/gachaponHelper.lua");

-- General rules:
-- No scrolls from the global items
-- Includes skill books for mastery level 20 for all classes

weight = mastery_book_20 * 4;

skills = {
	{2290000, ["weight"] = weight},
	{2290002, ["weight"] = weight},
	{2290004, ["weight"] = weight},
	{2290006, ["weight"] = weight},
	{2290008, ["weight"] = weight},
	{2290010, ["weight"] = weight},
	{2290012, ["weight"] = weight},
	{2290014, ["weight"] = weight},
	{2290016, ["weight"] = weight},
	{2290018, ["weight"] = weight},
	{2290019, ["weight"] = weight},
	{2290020, ["weight"] = weight},
	{2290022, ["weight"] = weight},
	{2290024, ["weight"] = weight},
	{2290026, ["weight"] = weight},
	{2290028, ["weight"] = weight},
	{2290030, ["weight"] = weight},
	{2290032, ["weight"] = weight},
	{2290034, ["weight"] = weight},
	{2290036, ["weight"] = weight},
	{2290038, ["weight"] = weight},
	{2290040, ["weight"] = weight},
	{2290042, ["weight"] = weight},
	{2290044, ["weight"] = weight},
	{2290046, ["weight"] = weight},
	{2290048, ["weight"] = weight},
	{2290050, ["weight"] = weight},
	{2290052, ["weight"] = weight},
	{2290054, ["weight"] = weight},
	{2290056, ["weight"] = weight},
	{2290058, ["weight"] = weight},
	{2290060, ["weight"] = weight},
	{2290062, ["weight"] = weight},
	{2290064, ["weight"] = weight},
	{2290066, ["weight"] = weight},
	{2290068, ["weight"] = weight},
	{2290070, ["weight"] = weight},
	{2290072, ["weight"] = weight},
	{2290074, ["weight"] = weight},
	{2290076, ["weight"] = weight},
	{2290078, ["weight"] = weight},
	{2290080, ["weight"] = weight},
	{2290082, ["weight"] = weight},
	{2290084, ["weight"] = weight},
	{2290086, ["weight"] = weight},
	{2290088, ["weight"] = weight},
	{2290090, ["weight"] = weight},
	{2290092, ["weight"] = weight},
	{2290094, ["weight"] = weight},
	{2290097, ["weight"] = weight},
	{2290099, ["weight"] = weight},
	{2290101, ["weight"] = weight},
	{2290102, ["weight"] = weight},
	{2290104, ["weight"] = weight},
	{2290106, ["weight"] = weight},
	{2290108, ["weight"] = weight},
	{2290110, ["weight"] = weight},
	{2290112, ["weight"] = weight},
	{2290114, ["weight"] = weight},
	{2290115, ["weight"] = weight},
	{2290117, ["weight"] = weight},
	{2290119, ["weight"] = weight},
	{2290121, ["weight"] = weight},
	{2290123, ["weight"] = weight},
	{2290124, ["weight"] = weight},
};

gachapon({
	["items"] = skills,
	["globalItemWeightModifier"] = function(item)
		-- Only pass non-scrolls through
		if math.floor(item["id"] / 10000) == 204 then
			return item_discard;
		end
		return item_keep;
	end,
});