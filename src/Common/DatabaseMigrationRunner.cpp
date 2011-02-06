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
#include "StringUtilities.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

DatabaseMigration::Runner::Runner(const string &filename) : m_filename(filename) {
	loadFile();
}

void DatabaseMigration::Runner::run() {
	mysqlpp::Query query = Database::getCharDB().query();
	std::stringstream x;
	for (size_t i = 0; i < m_queries.size(); i++) {
		try {
			query.exec(m_queries[i]);
		}
		catch (mysqlpp::BadQuery &ex) {
			std::cout << std::endl;
			std::cout << "ERROR: " << std::endl;
			std::cout << ex.what() << " (" << ex.errnum() << ")" << std::endl;
			std::cout << "File: " << m_filename << std::endl;
			getchar();
		}
	}

}

void DatabaseMigration::Runner::loadFile() {
	m_filestream.open(m_filename.c_str());

	// Read whole file
	{
		bool bulk = false;
		std::stringstream bulkQuery;
		std::ostringstream contentStream;
		while (!m_filestream.eof()) {
			string line;
			std::getline(m_filestream, line);
			if (line == "/* START BULK QUERY */") {
				// Starting of a bulk query (which has ;'s in it, just ignore them)
				bulk = true;
				// Reset the bulk query buffer
				bulkQuery.clear();
				bulkQuery.str("");
			}
			else if (line == "/* END BULK QUERY */") {
				// Ending of a bulk query
				bulk = false;
				m_queries.push_back(bulkQuery.str());
			}
			else if (bulk) {
				// Store data of a bulk query
				bulkQuery << line << std::endl;
			}
			else if (StringUtilities::hasEnding(line, ";")) {
				// Adding non-bulk queries into the m_queries vector
				if (!contentStream.str().empty()) {
					line = contentStream.str() + line;
				}
				m_queries.push_back(line);
				// Reset content stream
				contentStream.clear();
				contentStream.str("");
			}
			else {
				// Waiting till a ';' is found
				if (!line.empty()) {
					contentStream << line << std::endl;
				}
			}
		}

		if (!contentStream.str().empty()) {
			// For queries without an ending ;
			m_queries.push_back(contentStream.str());
		}
		if (bulk && !bulkQuery.str().empty()) {
			// Someone forgot to end a bulk query!
			std::cout << "Warning: Bulk query wasn't ended. File: " << m_filename << std::endl;
			m_queries.push_back(bulkQuery.str());
		}
	}
}
