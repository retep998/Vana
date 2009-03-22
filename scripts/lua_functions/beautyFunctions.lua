-- Various functions to make coding beauty parlour NPCs easier and minimise repeated code

skin = getSkin(); -- Player's current skin
hair = getHair(); -- Player's current hair
eye = getEyes(); -- Player's current eyes

-- Skin functions
function getSkins(styles)
	for i = 0, 4 do
		if not (currentSkin == i) then
			styles[i + 1] = i;
		end
	end
end

-- Hair functions
function getHairs(hairs, styles)
	colour = hair % 10;
	for i = 1, #hairs do
		if not (hairs[i] + colour == hair) then
			styles[#styles+1] = hairs[i] + colour;
		end
	end
end

function getHairColours(styles)
	cur = hair - hair % 10;
	for i = 0, 7 do
		if not (cur + i == hair) then
			styles[#styles+1] = cur + i;
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
function getEyeStyles(eyes, styles)
	colour = (eye % 1000) - (eye % 100);
	for i = 1, #eyes do
		if not (eyes[i] + colour == eye) then
			styles[#styles+1] = eyes[i] + colour;
		end
	end
end

function getEyeColour(styles)
	cur = eye - (eye % 1000 - eye % 100);
	for i = 0, 700, 100 do
		if not (cur + i == eye) then
			styles[#styles+1] = cur + i;
		end
	end
end

function giveRandomEyes(eyes)
	colour = (eye % 1000) - (eye % 100);
	newEyes = hairs[getRandomNumber(#eyes)] + colour;
	while (newEyes == eye) do
		newHair = hairs[getRandomNumber(#eyes)] + colour;
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
