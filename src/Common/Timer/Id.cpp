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
#include "Id.h"
#include <boost/functional/hash.hpp>

namespace Timer {

// For hashing Id
size_t hash_value(Id const &id) {
	size_t seed = 0;

	boost::hash_combine(seed, id.type);
	boost::hash_combine(seed, id.id);
	boost::hash_combine(seed, id.id2);

	return seed;
}

}
