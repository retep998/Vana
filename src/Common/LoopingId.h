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
#ifndef LOOPINGID_H
#define LOOPINGID_H

// LoopingId, keep track and returns a number between min and max, restarts when number reaches max
class LoopingId {
public:
	LoopingId(int minimum = 1, int maximum = INT_MAX);
	int next();
	void reset();
private:
	int current;
	int minimum;
	int maximum;
};

inline LoopingId::LoopingId(int minimum, int maximum) : current(minimum), minimum(minimum), maximum(maximum) { }

inline int LoopingId::next() {
	int ret = current;
	if (current == maximum) {
		current = minimum;
	}
	else {
		current++;
	}
	return ret;
}

inline void LoopingId::reset() {
	current = minimum;
}

#endif
