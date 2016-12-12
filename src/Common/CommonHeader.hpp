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
#pragma once

// Technically the packet for global sets the length, but your Maple locale may not be the same
// Search for IV_PATCH_LOCATION in order to find the spot where the connect packet is created if this is the case for your locale
#define IV_PATCH_LOCATION 0x0e

#define SMSG_PING 0x11
#define CMSG_PONG 0x19