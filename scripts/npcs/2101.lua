if state == 0 then
	addText("Are you done with your training? ");
	addText("If you wish, I will send you out from this training camp.");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		addText("Then, I will send you out from here. Good job.");
		sendNext();
	else
		addText("Haven't you finish the training program yet? ");
		addText("If you want to leave this place, please do not hesitate to tell me.");
		sendOK();
	end
elseif state == 2 then
	if getSelected() == 1 then
		setMap(3);
	end
	endNPC();
end
