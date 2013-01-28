/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "MySqlQueryParser.h"
#include "StringUtilities.h"
#include "tokenizer.hpp"
#include <fstream>

vector<string> MySqlQueryParser::parseQueries(const string &filename) {
	std::vector<string> queries;
	std::ifstream filestream;

	filestream.open(filename.c_str());

	string content;
	// Read whole file
	{
		std::ostringstream contentStream;
		while (!filestream.eof()) {
			string line;
			std::getline(filestream, line);
			contentStream << line << std::endl;
		}

		content = contentStream.str();
	}

	// Parse each SQL statement
	{
		MiscUtilities::tokenizer tokens(content, ";");

		for (MiscUtilities::tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
			string q = *iter;
			q = StringUtilities::trim(q);
			if (q.size() > 0) {
				queries.push_back(q);
			}
		}
	}

	return queries;
}