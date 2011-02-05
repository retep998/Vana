/*
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
*/
#pragma once

#include "GameConstants.h"
#include "Types.h"
#include <boost/array.hpp>
#include <string>

using std::string;

struct GuildLogo {
	GuildLogo() : logo(0), color(0), background(0), backgroundColor(0) { }
	GuildLogo(const GuildLogo &logo) : logo(logo.logo), color(logo.color), background(logo.background), backgroundColor(logo.backgroundColor) { }

	int8_t color;
	int8_t backgroundColor;
	int16_t logo;
	int16_t background;
};

typedef boost::array<string, GuildsAndAlliances::RankQuantity> GuildRanks;
