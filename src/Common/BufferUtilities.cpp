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
#include "BufferUtilities.h"

short BufferUtilities::getShort(unsigned char* buf){
	return buf[0] + buf[1]*0x100;
}

int BufferUtilities::getInt(unsigned char* buf){
	return buf[0] + buf[1]*0x100 + buf[2]*0x100*0x100 + buf[3]*0x100*0x100*0x100;
}

void BufferUtilities::getString(unsigned char* buf, int len, char* out){
	strncpy_s(out, len+1, (const char*)buf, len); 
}
