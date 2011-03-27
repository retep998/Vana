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
#pragma once

#include "Ip.h"
#include "PacketCreator.h"
#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class PacketReader;

namespace IpUtilities {
	ip_t stringToIp(const string &name);
	string ipToString(ip_t ip);
	ip_t matchIpSubnet(ip_t ip, const IpMatrix &ipMatrix, ip_t defaultIp = 0);

	// Used for receiving vectors of external IPs
	void extractExternalIp(PacketReader &packet, IpMatrix &extIp);
	// Used for passing vectors of external IPs
	struct SendIpArray {
	public:
		SendIpArray(PacketCreator &packet) : packet(packet) { }

		void operator()(const IpArray &ip) {
			packet.add<ip_t>(ip[0]); // IP
			packet.add<ip_t>(ip[1]); // Subnet
		}
	private:
		PacketCreator &packet;
	};
}