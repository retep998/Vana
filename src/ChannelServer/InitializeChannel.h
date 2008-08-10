/*
Copyright (C) 2008 Vana Development Team

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
*/
#ifndef INITIALIZECHANNEL_H
#define INITIALIZECHANNEL_H

namespace Initializing {
	void checkVEDBVersion();
	void initializeMobs();
	void initializeDrops();
	void initializeReactors();
	void initializeMaps();
	void initializeEquips();
	void initializeShops();
	void initializeItems();
	void initializeQuests();
	void initializeSkills();
	void initializePets();

	const int vedb_version = 16;
	const int vedb_subversion = 0;
};

#endif
