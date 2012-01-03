/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PacketReader.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

ip_t IpUtilities::stringToIp(const string &name) {
	boost::asio::io_service ioService;
	tcp::resolver resolver(ioService);
	tcp::resolver::query query(tcp::v4(), name, "http"); // Resolver wants a service...
	tcp::resolver::iterator iter = resolver.resolve(query);
	tcp::resolver::iterator end;

	// boost::asio throws an exception if the name cannot be resolved

	tcp::endpoint ep = *iter;
	return ep.address().to_v4().to_ulong();
}

string IpUtilities::ipToString(ip_t ip) {
	return boost::asio::ip::address_v4(ip).to_string();
}

ip_t IpUtilities::matchIpSubnet(ip_t ip, const IpMatrix &ipMatrix, ip_t defaultIp) {
	ip_t ret = defaultIp;

	for (IpMatrix::const_iterator iter = ipMatrix.begin(); iter != ipMatrix.end(); ++iter) {
		const IpArray &ipArray = *iter;
		ip_t serverIp = ipArray[0];
		ip_t subnet = ipArray[1];

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

		ip.push_back(packet.get<ip_t>());
		ip.push_back(packet.get<ip_t>());

		extIp.push_back(ip);
	}
}