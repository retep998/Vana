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
-- Various functions to make coding beauty parlour NPCs easier and minimise repeated code

skin = getSkin(); -- Player's current skin
hair = getHair(); -- Player's current hair
eye = getEyes(); -- Player's current eyes

-- Skin functions
function getSkins(styles)
	v = getAllSkins();
	for i = 1, #v do
		if not (currentSkin == v[i]) then
			styles[i] = v[i];
		end
	end
end

-- Hair functions
function getHairs(styles, hairs)
	colour = hair % 10;
	if hairs == nil then
		hairs = getAllHair();
	end
	for i = 1, #hairs do
		if (hairs[i] % 10 == 0) and not (hairs[i] + colour == hair) then
			if isValidHair(hairs[i] + colour) then
				styles[#styles + 1] = hairs[i] + colour;
			else
				styles[#styles + 1] = hairs[i];
			end
		end
	end
end

function getHairColours(styles)
	cur = hair - hair % 10;
	for i = 0, 9 do
		if isValidHair(cur + i) and not (cur + i == hair) then
			styles[#styles + 1] = cur + i;
		end
	end
end

function giveRandomHair(hairs)
	colour = hair % 10;
	newHair = hairs[getRandomNumber(#hairs)] + colour;
	while (newHair == hair) do
		newHair = hairs[getRandomNumber(#hairs)] + colour;
	end
	setStyle(newHair)
end

function giveRandomHairColour()
	cur = hair - hair % 10;
	colour = hair % 10;
	newColour = getRandomNumber(8) - 1;
	while (newColour == colour) do
		newColour = getRandomNumber(8) - 1;
	end
	setStyle(cur + newColour);
end

-- Eye functions
function getEyeStyles(styles, eyes)
	colour = (eye % 1000) - (eye % 100);
	if eyes == nil then
		eyes = getAllFaces();
	end
	for i = 1, #eyes do
		if ((eyes[i] % 1000 - eyes[i] % 100) == 0) and not (eyes[i] + colour == eye) then
			if isValidFace(eyes[i] + colour) then
				styles[#styles + 1] = eyes[i] + colour;
			else
				styles[#styles + 1] = eyes[i];
			end
		end
	end
end

function getEyeColour(styles)
	cur = eye - (eye % 1000 - eye % 100);
	for i = 0, 900, 100 do
		if isValidFace(cur + i) and not (cur + i == eye) then
			styles[#styles + 1] = cur + i;
		end
	end
end

function giveRandomEyes(eyes)
	colour = (eye % 1000) - (eye % 100);
	newEyes = eyes[getRandomNumber(#eyes)] + colour;
	while (newEyes == eye) do
		newEyes = eyes[getRandomNumber(#eyes)] + colour;
	end
	setStyle(newEyes)
end

function giveRandomEyeColour()
	cur = eye - (eye % 1000 - eye % 100);
	colour = (eye % 1000) - (eye % 100);
	newColour = (getRandomNumber(8) - 1) * 100;
	while (newColour == colour) do
		newColour = (getRandomNumber(8) - 1) * 100;
	end
	setStyle(cur + newColour);
end
