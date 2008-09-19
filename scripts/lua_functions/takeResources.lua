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