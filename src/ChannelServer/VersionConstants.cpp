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
#include "VersionConstants.h"
#include "MapleVersion.h"

int32_t TradeHandler::getTaxLevel(int32_t mesos) {
#if MAPLE_VERSION >= 67
	if (mesos < 100000)
		return 0;
	if (mesos >= 100000000)
		return 600;
	if (mesos >= 25000000)
		return 500;
	if (mesos >= 10000000)
		return 400;
	if (mesos >= 5000000)
		return 300;
	if (mesos >= 1000000)
		return 180;
	return 80;
#elif MAPLE_VERSION >= 17
	if (mesos < 50000)
		return 0;
	if (mesos >= 10000000)
		return 400;
	if (mesos >= 5000000)
		return 300;
	if (mesos >= 1000000)
		return 200;
	if (mesos >= 100000)
		return 100;
	return 50; /* MAPLE_VERSION check for tax levels */
#endif
	return 0; // There was no tax prior to .17
}