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