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

#include <boost/lexical_cast.hpp>
#include <string>
using std::string;

class CProcessorInfo {
public:
	CProcessorInfo() { ::GetSystemInfo(&m_sysInfo); }

	string GetProcessorName() const {
		string sRC;

		string sSpeed;
		string sVendor;

		// Get the processor speed info.
		HKEY hKey;
		LONG result = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);

		// Check if the function has succeeded.
		if (result == ERROR_SUCCESS) {
			DWORD data;
			DWORD dataSize = sizeof(data);
			result = ::RegQueryValueEx (hKey, "~MHz", NULL, NULL, (LPBYTE)&data, &dataSize);

			if (result == ERROR_SUCCESS) {
				sSpeed = boost::lexical_cast<string>(data) + "MHz";
			}
			else {
				sSpeed = "Unknown";
			}

			TCHAR vendorData [64];
			dataSize = sizeof (vendorData);

			result = ::RegQueryValueEx (hKey, "VendorIdentifier", NULL, NULL, (LPBYTE)vendorData, &dataSize);

			if (result == ERROR_SUCCESS) {
				sVendor = boost::lexical_cast<string>(vendorData);
			}
			else {
				sVendor = "Unknown";
			}
			// Make sure to close the reg key
			RegCloseKey (hKey);
		}
		else {
			sVendor = "Unknown";
			sSpeed = "Unknown";
		}


		LONG lRet;
		lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);
		if (lRet != ERROR_SUCCESS)
			return FALSE;
		TCHAR szTmp[2048];
		DWORD cntBytes = sizeof(szTmp);
		lRet = ::RegQueryValueEx(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)szTmp, &cntBytes);
		TCHAR* psz = szTmp;
		if (lRet != ERROR_SUCCESS) {
			psz = "Unknown";
		}
		else {
			::RegCloseKey(hKey);
			// Skip spaces
			while (iswspace(*psz))
				++psz;
		}

		sRC = "\r\n\tVendor:\t\t\t\t " + sVendor + "\r\n\tSpeed:\t\t\t\t " + sSpeed + "\r\n\tType:\t\t\t\t " + psz;

		return sRC;
	}
protected:
	SYSTEM_INFO m_sysInfo;
};

class CMemoryInfo {
public:
	string GetMemoryInfo() const {
		string sRC;

		MEMORYSTATUS memoryStatus;

		memset(&memoryStatus, sizeof(MEMORYSTATUS), 0);
		memoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&memoryStatus);

		// C-style size_t for GetProcessorWorkingSetSize()
		SIZE_T dwMinWSSize;
		SIZE_T dwMaxWSSize;

		::GetProcessWorkingSetSize(GetCurrentProcess(), &dwMinWSSize, &dwMaxWSSize);

		sRC = "\r\n\tMemory used:\t\t\t " + boost::lexical_cast<string>(memoryStatus.dwMemoryLoad) + "%";
		sRC += "\r\n\tTotal Physical Memory:\t\t " + boost::lexical_cast<string>(memoryStatus.dwTotalPhys / 1024) + "KB";
		sRC += "\r\n\tPhysical Memory Available:\t\t " + boost::lexical_cast<string>(memoryStatus.dwAvailPhys / 1024) + "KB";
		sRC += "\r\n\tTotal Virtual Memory:\t\t\t " + boost::lexical_cast<string>(memoryStatus.dwTotalVirtual / 1024) + "KB";
		sRC += "\r\n\tAvailable Virtual Memory:\t\t " +  boost::lexical_cast<string>(memoryStatus.dwAvailVirtual / 1024) + "KB";
		sRC += "\r\n\tWorking Set:";
		sRC += "\r\n\t\tMin:\t\t\t " + boost::lexical_cast<string>(dwMinWSSize/1024) + "KB";
		sRC += "\r\n\t\tMax:\t\t\t " + boost::lexical_cast<string>(dwMaxWSSize/1024) + "KB";
		sRC += "\r\n\r\n";
		return sRC;
	}
};
#endif