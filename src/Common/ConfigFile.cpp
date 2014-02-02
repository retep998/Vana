/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include "ExitCodes.hpp"
#include "FileUtilities.hpp"
#include <iostream>


ConfigFile::ConfigFile(const string_t &filename) :
	LuaEnvironment(filename)
{
}

auto ConfigFile::handleFileNotFound(const string_t &filename) -> void {
	std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
	ExitCodes::exit(ExitCodes::ConfigFileMissing);
}

auto ConfigFile::handleKeyNotFound(const string_t &filename, const string_t &key) -> void {
	std::cerr << "ERROR: Couldn't get a value from config file." << std::endl;
	std::cerr << "File: " << filename << std::endl;
	std::cerr << "Value: " << key << std::endl;
	ExitCodes::exit(ExitCodes::ConfigError);
}