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
#include "Database.h"
#include "DatabaseMigrationRunner.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

DatabaseMigration::Runner::Runner(const string &filename) :
	m_filename(filename)
{
	loadFile();
}

void DatabaseMigration::Runner::run() {
	mysqlpp::Query query = Database::getCharDb().query();

	for (size_t i = 0; i < m_queries.size(); i++) {
		query << m_queries[i];

		if (!query.exec()) {
			std::cout << "\nERROR: " << Database::getCharDb().error() << std::endl;
			std::cout << "File: " << m_filename << std::endl;
			// TODO: Handle the error
		}
	}
}

void DatabaseMigration::Runner::loadFile() {
	m_filestream.open(m_filename.c_str());

	string content;
	// Read whole file
	{
		std::ostringstream contentStream;
		while (!m_filestream.eof()) {
			string line;
			std::getline(m_filestream, line);
			contentStream << line << std::endl;
		}

		content = contentStream.str();
	}

	// Parse each SQL statement
	{
		typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
		typedef boost::char_separator<char> separator;

		separator sep(";");
		tokenizer tokens(content, sep);

		for (tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
			string query = boost::trim_copy(*iter);

			if (query.size() > 0) {
				m_queries.push_back(*iter);
			}
		}
	}
}