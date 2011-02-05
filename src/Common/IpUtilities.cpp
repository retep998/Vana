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
#include "IpUtilities.h"
#include "Ip.h"
#include "PacketReader.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

uint32_t IpUtilities::stringToIp(const string &name) {
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(tcp::v4(), name, "http"); // Resolver wants a service...
	tcp::resolver::iterator iter = resolver.resolve(query);
	tcp::resolver::iterator end;

	// boost::asio throws an exception if the name cannot be resolved

	tcp::endpoint ep = *iter;
	return ep.address().to_v4().to_ulong();
}

string IpUtilities::ipToString(uint32_t ip) {
	return boost::asio::ip::address_v4(ip).to_string();
}

uint32_t IpUtilities::matchIpSubnet(uint32_t ip, const IpMatrix &ipMatrix, uint32_t defaultIp) {
	uint32_t ret = defaultIp;

	for (IpMatrix::const_iterator iter = ipMatrix.begin(); iter != ipMatrix.end(); iter++) {
		const IpArray &ipArray = *iter;
		uint32_t serverIp = ipArray[0];
		uint32_t subnet = ipArray[1];

		if ((ip & subnet) == (serverIp & subnet)) {
			ret = serverIp;
			break;
		}
	}

	return ret;
}

void IpUtilities::extractExternalIp(PacketReader &packet, IpMatrix &extIp) {
	uint32_t ipSize = packet.get<uint32_t>();
	for (uint32_t i = 0; i < ipSize; i++) {
		IpArray ip;
		ip.reserve(2);

		ip.push_back(packet.get<uint32_t>());
		ip.push_back(packet.get<uint32_t>());

		extIp.push_back(ip);
	}
}
