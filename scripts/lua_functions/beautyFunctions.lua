-- Various functions to make coding beauty parlour NPCs easier and minimise repeated code

skin = getSkin(); -- Player's current skin
hair = getHair(); -- Player's current hair
eye = getEyes(); -- Player's current eyes

function getSkins(styles)
	for i = 0, 4 do
		if not (currentSkin == i) then
			styles[i + 1] = i;
		end
	end
end

function getHairs(hairs, styles)
	haircolour = hair % 10;
	for i = 1, #hairs do
		if not (hairs[i] + haircolour == hair) then
			styles[#styles+1] = hairs[i] + haircolour;
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

function getEyes(eyes, styles)
	eyecolour = (eye % 1000) - (eye % 100);
	for i = 1, #eyes do
		if not (eyes[i] + eyecolour == eye) then
			styles[#styles+1] = eyes[i] + eyecolour;
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
