/*
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
*/
#include "Levels.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "LevelsPacket.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "VersionConstants.h"
#include <boost/lexical_cast.hpp>
#include <string>

using std::string;