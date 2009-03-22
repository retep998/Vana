-- displayResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by item 4031138
-- Returns nothing, just displays the array
function displayResources (items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #reqs - 1, 2 do
		local item = reqs[index];
		local amt = reqs[index + 1];
		if item == 4031138 then -- Mesos are shown as item 4031138 (Sack of Money)
			addText("#v4031138# " .. amt .. " mesos");
		else
			if amt == 1 then
				addText("#v" .. item .. "# #t" .. item .. "#\r\n");
			else
				addText("#v" .. item .. "# " .. amt .. " #t" .. item .. "#s\r\n");
			end
		end
	end
end

-- giveResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Mesos are indicated by item 4031138
-- Returns nothing, just adds the items/mesos
function giveResources (items, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1 ,#items - 1, 2 do
		if items[index] == 4031138 then
			giveMesos((multiplier * items[index + 1]));
		else
			giveItem(items[index], (multiplier * items[index + 1]));
		end
	end
end

-- hasResources takes an array of requirements in the form of {itemid, amount, itemid, amount} and a multiplier
-- Returns true if the player has the resources or false if not
function hasResources (requisites, multiplier)
	local has = true;
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1, #requisites - 1, 2 do
		if requisites[index] == 4031138 then -- Mesos are shown as item 4031138 (Sack of Money)
			if getMesos() < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		else
			if getItemAmount(requisites[index]) < (requisites[index + 1] * multiplier) then
				has = false;
				break;
			end
		end
	end
	return has;
end

-- takeResources takes an array of items in the form of {itemid, amount, itemid, amount} and a multiplier
-- Returns nothing, just takes the items/mesos
function takeResources (requisites, multiplier)
	if multiplier == nil then
		multiplier = 1;
	end
	for index = 1 ,#requisites - 1, 2 do
		if requisites[index] == 4031138 then
			giveMesos((-1 * multiplier * requisites[index + 1]));
		else
			giveItem(requisites[index], (-1 * multiplier * requisites[index + 1]));
		end
	end
end
