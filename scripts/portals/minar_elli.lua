-- Ellinia <-> Leafre
if getItemAmount(4031346) > 0 then
	giveItem(4031346, -1);
	if getMap() == 240010100 then
		setMap(101010000, "minar00");
	else
		setMap(240010100, "elli00");
	end
	showMessage("The Magical Seed is spent and you are transferred to somewhere.", 5);
else
	showMessage("Magic Seed is needed to go through the portal.", 5);
end
