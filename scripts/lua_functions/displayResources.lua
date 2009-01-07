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