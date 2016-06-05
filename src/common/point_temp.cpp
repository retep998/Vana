/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "point_temp.hpp"

namespace vana {

point::point(game_coord x, game_coord y) :
	x{x},
	y{y}
{
}

auto point::is_origin() const -> bool {
	return x == 0 && y == 0;
}

auto point::move(game_coord x, game_coord y) const -> point {
	return point{this->x + x, this->y + y};
}

auto point::move_x(game_coord x) const -> point {
	return point{this->x + x, this->y};
}

auto point::move_y(game_coord y) const -> point {
	return point{this->x, this->y + y};
}

}