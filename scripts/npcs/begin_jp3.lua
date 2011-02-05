--[[
Copyright (C) 2008-2011 Vana Development Team

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
-- 

addText("Are you done with your training? ");
addText("If you wish, I will send you out from this training camp.");
yes = askYesNo();

if yes == 1 then
	addText("Then, I will send you out from here. Good job.");
	sendNext();
	
	setMap(3);
else
	addText("Haven't you finish the training program yet? ");
	addText("If you want to leave this place, please do not hesitate to tell me.");
	sendOK();
end
