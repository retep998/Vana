-- Thomas Swift
if getMap() == 100000000 then
	if state == 0 then
		addText("I can take you to Amoria Village. Are you ready to go?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("I hope you had a great time! See you around!");
			sendNext();
		else
			addText("Ok, feel free to hang around until you're ready to go!");
			sendOK();
			endNPC();
		end
	elseif state == 2 then
		setMap(680000000);
		endNPC();
	end
else	
	if state == 0 then
		addText("I can take you back to your original location. Are you ready to go?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("I hope you had a great time! See you around!");
			sendNext();
		else
			addText("Ok, feel free to hang around until you're ready to go!");
			sendNext();
			endNPC();
		end
	elseif state == 2 then
		setMap(100000000);
		endNPC();
	end
end
