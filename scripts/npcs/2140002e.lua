if questid == 3514 then
	if state == 0 then
		endQuest(3514);
		addText("Hah... I found that very amusing. Very well, I'll unfreeze your memories.");
		sendOK();
		endNPC();
	end
else
	endNPC();
end