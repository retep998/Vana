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
#ifdef _WIN32

#include <string>
using std::string;

class CExceptionReport {
public:
	CExceptionReport();
	~CExceptionReport();

	static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

private:
	static void CreateReport(PEXCEPTION_POINTERS pExceptionInfo);

	static void StackWalk(CONTEXT Context);
	static BOOL GetAddrDetails(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset);
	static bool writeMiniDump(PEXCEPTION_POINTERS pExceptionInfo);
	static LPTSTR GetExceptionString(DWORD dwCode);

	static void SuspendThreads();

	static void AddToReport(const WCHAR * pText);
	static void AddToReport(const char * pText);
	static void AddToReport(const string &pText);
	static void AddToReport(int number);
	static void AddToReportHex(_int64 number, int minDigits = 0);

	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousExceptionFilter;
	static TCHAR m_pLogFileName[MAX_PATH];
	static HANDLE m_hReportFile;
	static TCHAR m_pDmpFileName[MAX_PATH];
	static HANDLE m_hDumpFile;
	
	static BOOL m_bFirstRun;
};

#endif