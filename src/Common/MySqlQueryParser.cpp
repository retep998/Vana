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
#include "MySqlQueryParser.hpp"
#include "FileUtilities.hpp"
#include "StringUtilities.hpp"
#include "tokenizer.hpp"
#include <fstream>

auto MySqlQueryParser::parseQueries(const string_t &filename) -> vector_t<string_t> {
	if (!FileUtilities::fileExists(filename)) {
		throw std::runtime_error{"Query file doesn't exist: " + filename};
	}

	vector_t<string_t> queries;
	std::ifstream filestream;

	filestream.open(filename.c_str());

	string_t content;
	// Read whole file
	{
		out_stream_t contentStream;
		while (!filestream.eof()) {
			string_t line;
			std::getline(filestream, line);
			contentStream << line << std::endl;
		}

		content = StringUtilities::replace(contentStream.str(), "%%PREFIX%%", Database::makeCharTable(""));
	}

	// Parse each SQL statement
	{
		MiscUtilities::tokenizer tokens{content, ";"};

		for (const auto &token : tokens) {
			string_t q = token;
			q = StringUtilities::trim(q);
			if (!q.empty()) {
				queries.push_back(q);
			}
		}
	}

	return queries;
}