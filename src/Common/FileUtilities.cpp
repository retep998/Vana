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
#include "FileUtilities.hpp"
#include <sys/stat.h>

namespace Vana {

auto FileUtilities::fileExists(const string_t &file) -> bool {
	struct stat fileInfo;
	return (!stat(file.c_str(), &fileInfo)) != 0;
}

auto FileUtilities::removeExtension(const string_t &file) -> string_t {
	string_t ret = file;
	ret = ret.erase(ret.find_last_of('.'));
	return ret;
}

}