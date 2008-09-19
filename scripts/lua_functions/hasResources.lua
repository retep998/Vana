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