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
#ifdef _WIN32
#pragma once
#include <Windows.h>
#pragma comment(lib, "User32.lib")

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL DisplaySystemVersion(LPTSTR buffer) {
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	DWORD dwType;

	BOOL bOsVersionInfoEx;
	
	LPTSTR tmp = buffer;
	
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	if (!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) {
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *) &osvi)) {
			return false;
		}
	}
	
	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if(pGNSI != 0) {
		pGNSI(&si);
	}
	else {
		GetSystemInfo(&si);
	}
 
	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4) {
		if (osvi.dwMajorVersion == 6) {
			if (osvi.dwMinorVersion == 0) {
				if (osvi.wProductType == VER_NT_WORKSTATION) {
					tmp += sprintf(tmp, "Windows Vista ");
				}
				else {
					tmp += sprintf(tmp, "Windows Server 2008 ");
				}
			}

			if (osvi.dwMinorVersion == 1) {
				if(osvi.wProductType == VER_NT_WORKSTATION) {
					tmp += sprintf(tmp, "Windows 7 ");
				}
				else {
					tmp += sprintf(tmp, "Windows Server 2008 R2 ");
				}
			}


			pGPI = (PGPI)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetProductInfo");

			pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);
			tmp += sprintf(tmp, " (");
			switch(dwType) {
				case PRODUCT_ULTIMATE: tmp += sprintf(tmp, "Ultimate Edition"); break;
				//case PRODUCT_PROFESSIONAL: tmp += sprintf(tmp, "Professional"); break;
				case PRODUCT_HOME_PREMIUM: tmp += sprintf(tmp, "Home Premium Edition"); break;
				case PRODUCT_HOME_BASIC: tmp += sprintf(tmp, "Home Basic Edition"); break;
				case PRODUCT_ENTERPRISE: tmp += sprintf(tmp, "Enterprise Edition"); break;
				case PRODUCT_BUSINESS: tmp += sprintf(tmp, "Business Edition"); break;
				case PRODUCT_STARTER: tmp += sprintf(tmp, "Starter Edition"); break;
				case PRODUCT_CLUSTER_SERVER: tmp += sprintf(tmp, "Cluster Server Edition"); break;
				case PRODUCT_DATACENTER_SERVER: tmp += sprintf(tmp, "Datacenter Edition"); break;
				case PRODUCT_DATACENTER_SERVER_CORE: tmp += sprintf(tmp, "Datacenter Edition (core installation)"); break;
				case PRODUCT_ENTERPRISE_SERVER: tmp += sprintf(tmp, "Enterprise Edition"); break;
				case PRODUCT_ENTERPRISE_SERVER_CORE: tmp += sprintf(tmp, "Enterprise Edition (core installation)"); break;
				case PRODUCT_ENTERPRISE_SERVER_IA64: tmp += sprintf(tmp, "Enterprise Edition for Itanium-based Systems"); break;
				case PRODUCT_SMALLBUSINESS_SERVER: tmp += sprintf(tmp, "Small Business Server"); break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM: tmp += sprintf(tmp, "Small Business Server Premium Edition"); break;
				case PRODUCT_STANDARD_SERVER: tmp += sprintf(tmp, "Standard Edition"); break;
				case PRODUCT_STANDARD_SERVER_CORE: tmp += sprintf(tmp, "Standard Edition (core installation)"); break;
				case PRODUCT_WEB_SERVER: tmp += sprintf(tmp, "Web Server Edition"); break;
			}
			tmp += sprintf(tmp, ") ");
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
			if (GetSystemMetrics(SM_SERVERR2)) {
				tmp += sprintf(tmp, "Windows Server 2003 R2, ");
			}
			else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER) {
				tmp += sprintf(tmp, "Windows Storage Server 2003");
			}
			//else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER) {
			//	tmp += sprintf(tmp, "Windows Home Server");
			//}
			else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
				tmp += sprintf(tmp, "Windows XP Professional x64 Edition");
			}
			else {
				tmp += sprintf(tmp, "Windows Server 2003, ");
			}

			// Test for the server type.
			if (osvi.wProductType != VER_NT_WORKSTATION) {
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
						tmp += sprintf(tmp, "Datacenter Edition for Itanium-based Systems");
					}
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
						tmp += sprintf(tmp, "Enterprise Edition for Itanium-based Systems");
					}
				}
				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)	{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
						tmp += sprintf(tmp, "Datacenter x64 Edition");
					}
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
						tmp += sprintf(tmp, "Enterprise x64 Edition");
					}
					else {
						tmp += sprintf(tmp, "Standard x64 Edition");
					}
				}
				else {
					if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER) {
						tmp += sprintf(tmp, "Compute Cluster Edition");
					}
					else if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
						tmp += sprintf(tmp, "Datacenter Edition");
					}
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
						tmp += sprintf(tmp, "Enterprise Edition");
					}
					else if (osvi.wSuiteMask & VER_SUITE_BLADE) {
						tmp += sprintf(tmp, "Web Edition");
					}
					else {
						tmp += sprintf(tmp, "Standard Edition");
					}
				}
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
			tmp += sprintf(tmp, "Windows XP ");
			if (osvi.wSuiteMask & VER_SUITE_PERSONAL) {
				tmp += sprintf(tmp, "Home Edition");
			}
			else {
				tmp += sprintf(tmp, "Professional");
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
			tmp += sprintf(tmp, "Windows 2000 ");

			if (osvi.wProductType == VER_NT_WORKSTATION) {
				tmp += sprintf(tmp, "Professional");
			}
			else {
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
					tmp += sprintf(tmp, "Datacenter Server");
				}
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
					tmp += sprintf(tmp, "Advanced Server");
				}
				else {
					tmp += sprintf(tmp, "Server");
				}
			}
		}

		// Include service pack (if any) and build number.

		if (strlen(osvi.szCSDVersion) > 0) {
			tmp += sprintf(tmp, " ");
			tmp += sprintf(tmp, osvi.szCSDVersion);
		}

		tmp += sprintf(tmp, " (build %d)", osvi.dwBuildNumber);

		if (osvi.dwMajorVersion >= 6) {
			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
				tmp += sprintf(tmp, " (64-bit)");
			}
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
				tmp += sprintf(tmp, " (32-bit)");
			}
		}
	}
	return true; 
}
#endif