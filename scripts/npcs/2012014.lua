-- Orbis Magic Spot (Orbis Tower: 20th Floor)
if getItemAmount(4001019) >= 1 then
	if state == 0 then
		addText("You can use #bOrbis Rock Scroll#k to activate #b Orbis Magic Spot.#k ");
		addText("Will you teleport to where #bEl Nath Magic Spot#k is?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			giveItem(4001019, -1);
			setMap(200082100);
			endNPC();
		else
			endNPC();
		end
	end
elseif getItemAmount(4001019) == 0 then
	if state == 0 then
		addText("There's a #bOrbis Magic Spot#k that'll enable you to teleport ");
		addText("to where #bEl Nath Magic Spot#k is, but you can't activate ");
		addText("it without the scroll.");
		sendOK();
		endNPC();
	end
end
