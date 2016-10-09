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
-- Various functions to make coding beauty parlour NPCs easier and minimise repeated code

dofile("scripts/utils/tableHelper.lua");

-- TODO FIXME beauty
-- The usage of "styles" vs not is inconsistent and confusing, consider better naming

--[[ TODO FIXME beauty
Review how some of these work...
There's a lot of boilerplate and inconsistency

For example, in lens_henesys1:
validFaceColours = {face_amethyst, face_black, face_blue, face_hazel, face_sapphire, face_violet};
-- EXP
				setStyle(getRandomFaceColour(validFaceColours));
-- VIP
		validFaceColours = getFaceColours(validFaceColours);
		choice = askStyle(validFaceColours);

		if giveItem(vipItem, -1) then
			setStyle(selectChoice(validFaceColours, choice));

In face_henesys2:
validFaces = getGenderStyles({
	["male"] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012, 20014},
	["female"] = {21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21012, 21014},
});
		setStyle(getRandomFace(validFaces));

In face_henesys1:
validFaces = getGenderStyles({
	["male"] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20012, 20014},
	["female"] = {21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21012, 21014},
});
validFaces = getFaceStyles(validFaces)
choice = askStyle(validFaces);

if giveItem(item, -1) then
	setStyle(selectChoice(validFaces, choice));

I would like it to be as simple as possible to support both of these patterns using the same underlying data/function calls without having to prepare them.
However, this is a bit tricky. EXP and VIP should work the same with minimal extra error-prone function calls if possible.
It needs a review again in the future, I was just in here cleaning this file up a little bit since it was basically useless before and moreover, not used by our scripts either.
--]]

-- Gendered functions
function getGenderStyles(tbl)
	function getOrDefault(res)
		if res == nil then
			return {};
		end
		return res;
	end

	if getGender() == gender_male then
		return getOrDefault(tbl["male"]);
	end
	return getOrDefault(tbl["female"]);
end

-- Skin constants
skin_light = 0;
skin_tanned = 1;
skin_dark = 2;
skin_pale = 3;
skin_blue = 4;
skin_green = 5;
skin_white = 9;
-- Not sure why this is in .75, but maybe it's different in some other version
-- skin_pale2 = 10;
skin_all = {
	skin_light, skin_tanned, skin_dark, skin_pale,
	skin_blue, skin_green, skin_white
};

-- Skin functions
function getSkinStyles(validSkins)
	local currentSkin = getSkin();
	if validSkins == nil then
		validSkins = getAllSkins();
	end
	local styles = {};
	for i = 1, #validSkins do
		local skinId = validSkins[i];
		if skinId ~= currentSkin then
			append(styles, skinId);
		end
	end
	return styles;
end

-- Hair constants
hair_black = 0;
hair_red = 1;
hair_orange = 2;
hair_blonde = 3;
hair_green = 4;
hair_blue = 5;
hair_purple = 6;
hair_brown = 7;
hair_all = {
	hair_black, hair_red, hair_orange, hair_blonde,
	hair_green, hair_blue, hair_purple, hair_brown
};

-- Hair functions
function getHairColour(hairId)
	return hairId % 10;
end

function getBaseHair(hairId)
	return hairId - getHairColour(hairId);
end

function isBaseHair(hairId)
	return getHairColour(hairId) == hair_black;
end

function buildHair(baseHairId, colourId)
	return baseHairId + colourId;
end

-- The expand functions are mostly useful if you want to change colors and styles at the same time
function expandAllHairs(baseHairIds, colours)
	local styles = {};
	for i = 1, #baseHairIds do
		append(expandHair(baseHairIds[i], colours));
	end
	return styles;
end

function expandHair(baseHairId, colours)
	local styles = {};
	if colours == nil then
		colours = hair_all;
	end
	for i = 1, #colours do
		append(styles, buildHair(baseHairId, colours[i]));
	end
	return styles;
end

function getHairStyles(validHairs)
	local currentHair = getHair();
	local currentHairColour = getHairColour(currentHair);
	if validHairs == nil then
		validHairs = getAllHairs();
	end
	local styles = {};
	for i = 1, #validHairs do
		local hairId = validHairs[i];
		local composed = buildHair(hairId, currentHairColour);
		if isBaseHair(hairId) and composed ~= currentHair then
			if isValidHair(composed) then
				append(styles, composed);
			else
				append(styles, hairId);
			end
		end
	end
	return styles;
end

function getHairColours(validColours)
	if validColours == nil then
		validColours = hair_all;
	end

	local currentHair = getHair();
	local baseHair = getBaseHair(currentHair);
	local currentHairColour = getHairColour(currentHair);
	local styles = {};
	for i = 1, #validColours do
		local composed = buildHair(baseHair, validColours[i]);
		if isValidHair(composed) and composed ~= currentHair then
			append(styles, composed);
		end
	end
	return styles;
end

-- Preferring color may lead to an infinite loop in the case where there are no hairs that match the current color, care must be taken to prevent this
-- The default mode is to prefer style since this has no infinite loops and is global-accurate
hair_prefer_color = 1;
hair_prefer_style = nil;
function getRandomHair(validHairs, hairPreference)
	if #validHairs <= 1 then
		return;
	end

	local currentHair = getHair();
	local currentHairColour = getHairColour(currentHair);
	local newHair = nil;
	repeat
		local baseHair = selectElement(validHairs);
		newHair = buildHair(baseHair, currentHairColour);
		if not isValidHair(newHair) and hairPreference == hair_prefer_style then
			newHair = baseHair;
		end
	until newHair ~= currentHair and isValidHair(newHair);

	return newHair;
end

function getRandomHairColour(validHairs)
	if validHairs == nil then
		validHairs = getHairColours();
	end

	if #validHairs <= 1 then
		return;
	end

	local currentHair = getHair();
	local newHair = nil;
	repeat
		newHair = selectElement(validHairs);
	until newHair ~= currentHair and isValidHair(newHair);

	return newHair;
end

-- Face constants
face_black = 000;
face_blue = 100;
face_red = 200;
face_green = 300;
face_hazel = 400;
face_sapphire = 500;
face_violet = 600;
face_amethyst = 700;
face_white = 800;
face_all = {
	face_black, face_blue, face_red, face_green,
	face_hazel, face_sapphire, face_violet, face_amethyst,
	face_white
};

-- Face functions
function getFaceColour(faceId)
	return (faceId % 1000) - (faceId % 100);
end

function getBaseFace(faceId)
	return faceId - getFaceColour(faceId);
end

function isBaseFace(faceId)
	return getFaceColour(faceId) == 0;
end

function buildFace(baseFaceId, colourId)
	return baseFaceId + colourId;
end

function getFaceStyles(validFaces)
	local currentFace = getFace();
	local currentFaceColour = getFaceColour(currentFace);
	if validFaces == nil then
		validFaces = getAllFaces();
	end
	local styles = {};
	for i = 1, #validFaces do
		local faceId = validFaces[i];
		local composed = buildFace(faceId, currentFaceColour);
		if isBaseFace(faceId) and composed ~= currentFace then
			if isValidFace(composed) then
				append(styles, composed);
			else
				append(styles, faceId);
			end
		end
	end
	return styles;
end

function getValidFaceColoursForStyle(faceId)
	local baseFace = getBaseFace(faceId);
	local styles = {};
	for i = 0, 900, 100 do
		local composed = buildFace(baseFace, i);
		if isValidFace(composed) and composed ~= currentFace then
			append(styles, i);
		end
	end
	return styles;
end

function getFaceColours(validFaceColours)
	local currentFace = getFace();
	local currentBaseFace = getBaseFace(currentFace);

	if validFaceColours == nil then
		validFaceColours = getValidFaceColoursForStyle(currentFace);
	end

	local styles = {};
	for i = 1, #validFaceColours do
		local color = validFaceColours[i];
		local composed = buildFace(currentBaseFace, color);
		if composed ~= currentFace then
			if isValidFace(composed) then
				append(styles, composed);
			else
				append(styles, currentBaseFace);
			end
		end
	end
	return styles;
end

-- Preferring color may lead to an infinite loop in the case where there are no faces that match the current color, care must be taken to prevent this
-- The default mode is to prefer style since this has no infinite loops and is global-accurate
face_prefer_color = 1;
face_prefer_style = nil;
function getRandomFace(validFaces, facePreference)
	if #validFaces <= 1 then
		return;
	end

	local currentFace = getFace();
	local currentFaceColour = getFaceColour(currentFace);
	local newFace = nil;
	repeat
		local baseFace = selectElement(validFaces);
		newFace = buildFace(baseFace, currentFaceColour);
		if not isValidFace(newFace) and facePreference == face_prefer_style then
			newFace = baseFace;
		end
	until newFace ~= currentFace and isValidFace(newFace);
	return newFace;
end

function getRandomFaceColour(validFaceColours)
	local currentFace = getFace();
	local baseFace = getBaseFace(currentFace);

	if validFaceColours == nil then
		validFaceColours = getValidFaceColoursForStyle(currentFace);
	end

	if #validFaceColours <= 1 then
		return;
	end

	local newFace = nil;
	repeat
		newFace = buildFace(baseFace, selectElement(validFaceColours));
	until newFace ~= currentFace and isValidFace(newFace);
	return newFace;
end