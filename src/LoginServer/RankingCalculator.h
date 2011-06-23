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

#include "Types.h"
#include <string>

using std::string;

namespace RankingCalculator {
	void setTimer();
	void runThread();
	void all();
	void overall();
	void world();
	void job();
	void fame();

	extern const string VariableDefinition;

	// I use a duplicate arrangement here to promote a balance of encapsulation and performance - but it's really ugly
	// That is, JobClause just calls the jobClause function once (and RankIfClause will do the same)
	// Which will build out the constant using a stream so I can use my core constants for jobs and player levels
	const string jobClause();
	const string rankIfClause();
	extern const string JobClause;
	extern const string RankIfClause;

	const int32_t JobTrackCount = 19;
	const int32_t BeginnerJobCount = 5;
	extern const int8_t JobTracks[JobTrackCount];
	extern const int16_t BeginnerJobs[BeginnerJobCount];
	extern const int16_t EvanBeginner;
}