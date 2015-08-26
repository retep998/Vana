/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Point.hpp"

Point::Point(coord_t x, coord_t y) :
	x{x},
	y{y}
{
}

auto Point::isOrigin() const -> bool {
	return x == 0 && y == 0;
}

auto Point::offset(coord_t x, coord_t y) const -> Point {
	return Point{this->x + x, this->y + y};
}

auto Point::offsetX(coord_t x) const -> Point {
	return Point{this->x + x, this->y};
}

auto Point::offsetY(coord_t y) const -> Point {
	return Point{this->x, this->y + y};
}