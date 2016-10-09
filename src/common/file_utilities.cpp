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
#include "file_utilities.hpp"
#include <sys/stat.h>

namespace vana {
namespace utilities {
namespace file {

auto exists(const string &file) -> bool {
	struct stat file_info;
	return (!stat(file.c_str(), &file_info)) != 0;
}

auto remove_extension(const string &file) -> string {
	string ret = file;
	ret = ret.erase(ret.find_last_of('.'));
	return ret;
}

}
}
}