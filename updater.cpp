/*
Source File : AutoUpdate.cpp
Created for the purpose of demonstration for http://www.codeproject.com

Copyright 2017 Michael Haephrati, Secured Globe Inc.
See also: https://www.codeproject.com/script/Membership/View.aspx?mid=5956881

Secured Globe, Inc.
http://www.securedglobe.com

*/

#include "stdafx.h"
#include "updater.h"
#include <urlmon.h>    
#include <iostream>

#pragma comment (lib, "urlmon.lib")
#pragma comment(lib, "Version.lib")

// Utilities
BOOL SG_Run(LPWSTR FileName)
{
	wprintf(L"Called SG_Run '%s'\n", FileName);
	PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter

	STARTUPINFO StartupInfo; //This is an [in] parameter

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

	if (CreateProcess(FileName, NULL,
		NULL, NULL, FALSE, 0, NULL,  
		NULL, &StartupInfo, &ProcessInfo))
	{
		//WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
		wprintf(L"Success\n");
		return TRUE;
	}
	else
	{
		wprintf(L"Failed\n");
		return FALSE;
	}

}

CString GetFileNameFromPath(CString Path)
{
	int slash = Path.ReverseFind(L'\\');
	return(Path.Mid(slash + 1));
}

// AutoUpdate class

AutoUpdate::AutoUpdate()
{
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		wprintf(L"Can't find module file name (%s)\n", GetLastError());
		return;
	}
	SetSelfFullPath(szPath);
	SetSelfFileName(GetFileNameFromPath(szPath));

	SG_Version ver;
	if (SG_GetVersion(szPath, &ver))
	{
		CString ModifiedFileName = szPath;
		AddNextVersionToFileName(ModifiedFileName, ver);
	}

	
	ReplaceTempVersion();

}
void AutoUpdate::AddNextVersionToFileName(CString& ExeFile, SG_Version ver)
{
	CString strVer;
	ver.SubRevision += 1;	// For the time being we just promote the subrevision in one but of course
	// we should build a mechanism to promote the major, minor and revision
	ExeFile = GetSelfFileName();
	ExeFile = ExeFile.Left(ExeFile.GetLength() - 4);
	strVer.Format(L"%d.%d.%d.%d", ver.Major, ver.Minor, ver.Revision, ver.SubRevision);
	ExeFile += L"." + strVer;
	ExeFile += L".exe";
	m_NextVersion = ExeFile;
}

void SaveVersionToString(const struct Version* ver) {
	// Format the string and save it to the global variable
	
}

BOOL AutoUpdate::SG_GetVersion(LPWSTR ExeFile, SG_Version* ver)
{
	BOOL result = FALSE;
	DWORD dwDummy;
	DWORD dwFVISize = GetFileVersionInfoSize(ExeFile, &dwDummy);
	LPBYTE lpVersionInfo = new BYTE[dwFVISize];
	GetFileVersionInfo(ExeFile, 0, dwFVISize, lpVersionInfo);
	UINT uLen;
	VS_FIXEDFILEINFO* lpFfi;
	VerQueryValue(lpVersionInfo, _T("\\"), (LPVOID*)&lpFfi, &uLen);
	if (lpFfi && uLen)
	{
		DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
		DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;
		delete[] lpVersionInfo;
		ver->Major = HIWORD(dwFileVersionMS);
		ver->Minor = LOWORD(dwFileVersionMS);
		ver->Revision = HIWORD(dwFileVersionLS);
		ver->SubRevision = LOWORD(dwFileVersionLS);
		sprintf(VersionString, "%d.%d.%d.%d\n", ver->Major, ver->Minor, ver->Revision, ver->SubRevision);
		
		result = TRUE;
	}
	else
		wprintf(L"Can't detect current version\n");
	return result;
}

AutoUpdate::~AutoUpdate()
{
}


CString AutoUpdate::GetSelfFullPath()
{
	return m_SelfFullPath;
}

void AutoUpdate::SetSelfFullPath(CString Path)
{
	m_SelfFullPath = Path;
}
CString AutoUpdate::GetSelfFileName()
{
	return m_SelfFileName;
}

void AutoUpdate::SetSelfFileName(CString FileName)
{
	m_SelfFileName = FileName;
}


BOOL AutoUpdate::ReplaceTempVersion()
{
	int tries = 5;
	if (m_SelfFileName.Left(3) == L"_U_")
	{
		tempversion = true;
		wprintf(L"We are running a temp version\n");
	retry:;
		BOOL result = DeleteFile(m_SelfFileName.Mid(3));
		if (result)
		{
			wprintf(L"File '%s' deleted\n", m_SelfFileName.Mid(3));
			BOOL result2 = CopyFile(m_SelfFileName, m_SelfFileName.Mid(3), FALSE);
			if (result2)
			{
				wprintf(L"File '%s' copied to '%s'\n", m_SelfFileName, m_SelfFileName.Mid(3));
				if (SG_Run(m_SelfFileName.Mid(3).GetBuffer()))
				{
					wprintf(L"Terminated %s\n", m_SelfFileName);
					_exit(0);
				}


			}
		}
		else
		{
			if (--tries) goto retry;
			wprintf(L"'original version' ('%s') can't be deleted or doesn't exists\n", m_SelfFileName.Mid(3));
		}
	}
	else
	{
		tempversion = false;
		wprintf(L"We are running the normal version\n");
	retry2:;
		BOOL result = DeleteFile(L"_U_" + m_SelfFileName);
		if (result)
		{
			wprintf(L"temp File '%s' deleted\n", L"_U_" + m_SelfFileName);
		}
		else
		{
			if (--tries) goto retry2;
			wprintf(L"temp File '%s' can't be deleted or doesn't exist\n", L"_U_" + m_SelfFileName);

		}

	}
	return TRUE;
}
BOOL AutoUpdate::SG_VersionMatch(CString ExeFile, SG_Version* ver)
{
	BOOL result = false;
	int major, minor, revision, subrevision;
	int last_char = ExeFile.GetLength();
	int i1, i2, i3, i4, i5;
	CString curString = ExeFile;
	i1 = curString.ReverseFind(L'.');
	if (i1 > -1)
	{
		CString strStart = ExeFile.Mid(0, i1);
		i2 = strStart.ReverseFind(L'.');
		curString = curString.Left(i2);
		if (i2 > -1)
		{
			CString StrSubrevision = ExeFile.Mid(i2 + 1, i1 - i2 - 1);
			subrevision = _wtoi(StrSubrevision.GetBuffer());
			i3 = curString.ReverseFind(L'.');
			curString = curString.Left(i3);
			if (i3 > -1)
			{
				CString StrRevision = ExeFile.Mid(i3 + 1, i2 - i3 - 1);
				revision = _wtoi(StrRevision.GetBuffer());
				i4 = curString.ReverseFind(L'.');
				curString = curString.Left(i4);
				if (i4 > -1)
				{
					CString StrMinor = ExeFile.Mid(i4 + 1, i3 - i4 - 1);
					minor = _wtoi(StrMinor.GetBuffer());
					i5 = curString.ReverseFind(L'.');
					curString = curString.Left(i5);
					if (i5 > -1)
					{
						CString StrMajor = ExeFile.Mid(i5 + 1, i4 - i5 - 1);
						major = _wtoi(StrMajor.GetBuffer());
					}
					if (major == ver->Major &&
						minor == ver->Minor &&
						revision == ver->Revision &&
						subrevision == ver->SubRevision)
						result = TRUE;
				}

			}

		}
	}
	return result;
}
BOOL AutoUpdate::CheckForUpdates(void)
{
	

	if (tempversion) return TRUE;	// We don't check for updates if we are running a temp version
	MyCallback pCallback;
	CString ExeName = L"_U_" + m_SelfFileName;
	CString URL = m_DownloadLink + m_NextVersion;
	wprintf(L"Next version will be %s\n", m_NextVersion);
	if (m_NextVersion == L"") return FALSE;
	wprintf(L"Looking for updates at %s\n", URL);
	DeleteUrlCacheEntry(URL);
	HRESULT hr = 0;
	hr = URLDownloadToFile(
		NULL,   // A pointer to the controlling IUnknown interface (not needed here)
		URL,
		ExeName, 0,		      // Reserved. Must be set to 0.
		&pCallback); // status callback interface (not needed for basic use)
	if (SUCCEEDED(hr))
	{
		// Check if the version string matches the file name on the server
		SG_Version ver;
		std::cout << "TESTS" << &ver << std::endl;
		if (SG_GetVersion(ExeName.GetBuffer(), &ver))
		{
			if (SG_VersionMatch(m_NextVersion.GetBuffer(), &ver) == FALSE)
			{
				wprintf(L"Version string doesn't match actual version\n");
				return FALSE;
			}
		}
		wprintf(L"Downloaded file '%s' which is a newer version. Result = %u\n", m_NextVersion, hr);

		if (SG_Run(ExeName.GetBuffer()))
		{
			wprintf(L"Successfully started the temp version (%s)\n", ExeName);
			_exit(0);
		}
		else
		{
			wprintf(L"Couldn't start the temp version (%s)\n", ExeName);
		}

	}
	else
		wprintf(L"No new version (%s) on the server\n", m_NextVersion);
	return (hr) ? TRUE : FALSE;
}






