-- Pison - Tour Guide
-- Teleports from Florina Beach
if state == 0 then
	origin = getPlayerVariable("florina_origin");
	
	addText("So you want to leave #b#m110000000##k? If you want, I can take you back to #b#m" .. origin .. "##k.\r\n");
	addText("#b#L0# #b I would like to go back now.");
	sendSimple();
elseif state == 1 then
	addText("Are you sure you want to return to #b#m" .. origin .. "##k? Alright, we'll have to get going fast. Do you want to head back to #m" .. origin .. "# now?");
	sendYesNo();
elseif state == 2 then
	what = getSelected();
	if what == 1 then
		deletePlayerVariable("florina_origin");
		setMap(origin);
		endNPC();
	else
		addText("You must have some business to take care of here. It's not a bad idea to take some rest at #m" .. origin .. "#. Look at me; I love it here so much that I wound up living here. Hahaha ¡¦ anyway, talk to me when you feel like going back.");
		sendNext();
	end
else
	endNPC();
end