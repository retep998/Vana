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
#include "mysql_query_parser.hpp"
#include "common/util/file.hpp"
#include "common/util/string.hpp"
#include "common/util/tokenizer.hpp"
#include <fstream>

namespace vana {
namespace io {
namespace mysql_query_parser {

auto parse_queries(const string &filename) -> vector<string> {
	if (!vana::util::file::exists(filename)) {
		throw std::runtime_error{"Query file doesn't exist: " + filename};
	}

	vector<string> queries;
	std::ifstream filestream;

	filestream.open(filename.c_str());

	auto &db = vana::io::database::get_char_db();
	string content;
	// Read whole file
	{
		out_stream content_stream;
		while (!filestream.eof()) {
			string line;
			std::getline(filestream, line);
			content_stream << line << std::endl;
		}

		content = vana::util::str::replace(content_stream.str(), "%%PREFIX%%", db.get_table_prefix());
	}

	// Parse each SQL statement
	{
		vana::util::tokenizer tokens{content, ";"};

		for (const auto &token : tokens) {
			string q = token;
			q = vana::util::str::trim(q);
			if (!q.empty()) {
				queries.push_back(q);
			}
		}
	}

	return queries;
}

}
}
}