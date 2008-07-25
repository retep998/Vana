-- El Nath Magic Spot (Orbis Tower: 1st Floor)
if getItemAmount(4001019) >= 1 then
	if state == 0 then
		addText("You can use #b#t4001019##k to activate #b#p2012015##k. Will you teleport to where #b#p2012014##k is?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			giveItem(4001019, -1);
			setMap(200080200);
		end
		endNPC();
	end
else
	if state == 0 then
		addText("There's a #b#p2012015##k that'll enable you to teleport to where #b#p2012014##k is, but you can't activate it without the scroll.");
		sendOK();
		endNPC();
	end
end
