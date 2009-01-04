--[[
Copyright (C) 2008-2009 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
--]]
-- Jeff - Ice Valley II
if state == 0 then
	addText("Hey, you look like you want to go farther and deeper past this place. Over there, though, you'll find yourself surrounded by aggressive, dangerous monsters, so even if you feel that you're ready to go, please be careful. Long ago, a few brave men from our town went in wanting to eliminate anyone threatening the town, but never came back out...");
	sendNext();
elseif state == 1 then
	addText("If you are thinking of going in, I suggest you change your mind. But if you really want to go in...I'm only letting in the ones that are strong enough to stay alive in there. I do not wish to see anyone else die. Let's see ... Hmmm ...");
	if getLevel() < 50 then
		addText(" you haven't reached Level 50 yet. I can't let you in, then, so forget it.");
		sendBackOK();
	else
		addText("! You look pretty strong. All right, do you want to go in?");
		sendYesNo();
	end
elseif state == 2 then
	if getLevel() < 50 then
		endNPC();
	elseif getSelected() == 1 then
		setMap(211040300, "sp");
	else
		addText("Even if your level's high it's hard to actually go in there, but if you ever change your mind please find me. After all, my job is to protect this place.");
		sendNext();
	end
	endNPC();
end
