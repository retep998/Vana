-- Ellinia <-> Leafre
if getItemAmount(4031346) > 0 then
	giveItem(4031346, -1);
	if getMap() == 240010100 then
		setPortalToId(101010000);
		setPortalTo("minar00");
	else
		setPortalToId(240010100);
		setPortalTo("elli00");
	end
	showMessage("The Magical Seed is spent and you are transferred to somewhere.", 5);
else
	showMessage("Magic Seed is needed to go through the portal.", 5);
	setPortalToId(-1);
end
