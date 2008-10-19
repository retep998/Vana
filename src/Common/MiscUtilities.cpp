/*
Copyright (C) 2008 Vana Development Team

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
#include "MiscUtilities.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

string MiscUtilities::nameToIP(string &name) {
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service); 
	tcp::resolver::query query(tcp::v4(), name, "http"); // Resolver wants a service...
	tcp::resolver::iterator iter = resolver.resolve(query); 
	tcp::resolver::iterator end; 

	// boost::asio throws an exception if the name cannot be resolved

	tcp::endpoint ep = *iter;
	return ep.address().to_string();
}